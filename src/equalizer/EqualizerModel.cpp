#include "EqualizerModel.h"

#include "AudioFilter.h"
#include "FilterModel.h"
#include <measure/MeasurementManager.h>
#include <target/TargetModel.h>

#include <rxcpp/operators/rx-combine_latest.hpp>

// https://www.sonarworks.com/soundid-reference/blog/learn/eq-curves-defined/

EqualizerModel::EqualizerModel(const TargetModel& targetModel,
                               QObject *parent)
    : ChartModel(parent),
      _targetModel(targetModel),
      _range({3, 28}) {
    setType(FrequencyResponse);
    FrequencyTable<double> table;
    _frequencyTable = table.frequencies();
    FrequencyTable<double> rangeTable(3);
    _rangeTable = rangeTable.frequencies();

    MeasurementManager::instance().calibratedFr()
            .combine_latest([this](const auto& measurement, const auto& filter) {
        std::vector<double> out;
        out.reserve(measurement.size());
        for (int i = 0; i < measurement.size(); ++i) {
            out.push_back(measurement.at(i) + filter.at(i));
        }
        return out;
    }, _sum.get_observable())
            .subscribe([this](const auto& filteredMeasurement) {
        _filtered = filteredMeasurement;
        QVector<QPointF> points;
        points.reserve(_frequencyTable.size());
        for (int i = 0; i < _frequencyTable.size(); ++i) {
            points.append( { _frequencyTable.at(i), filteredMeasurement.at(i) } );
        }
        _filteredSeries->replace(points);
    });

    _sum.get_observable()
            .subscribe([this](const auto& sum) {
        QVector<QPointF> points;
        points.reserve(_frequencyTable.size());
        for (int i = 0; i < _frequencyTable.size(); ++i) {
            points.append( { _frequencyTable.at(i), sum.at(i) } );
        }
        _sumSeries->replace(points);
    });

    _targetModel.fr()
            .combine_latest([this](const auto& sum, auto level) {
        std::vector<double> out;
        out.reserve(sum.size());
        for (int i = 0; i < sum.size(); ++i) {
            out.push_back(sum.at(i) + level);
        }
        return out;
    }, _level.get_observable())
            .subscribe([this](const auto& sum) {
        _target = sum;
        QVector<QPointF> points;
        points.reserve(_frequencyTable.size());
        for (int i = 0; i < _frequencyTable.size(); ++i) {
            points.append( { _frequencyTable.at(i), sum.at(i) } );
        }
        _targetSeries->replace(points);
    });
}

double EqualizerModel::minFrequencySlider() const {
    return _range.get_value().first;
}

double EqualizerModel::maxFrequencySlider() const {
    return _range.get_value().second;
}

void EqualizerModel::setMinFrequencySlider(double value) {
    auto curr = _range.get_value();
    curr.first = std::min(value, (double)(_rangeTable.size()-2));
    if (curr.second <= curr.first) {
        curr.second = curr.first + 1;
    }
    _range.get_subscriber().on_next(curr);
    emit rangeChanged();
}

void EqualizerModel::setMaxFrequencySlider(double value) {
    auto curr = _range.get_value();
    curr.second = std::max(value, 1.0);
    if (curr.first >= curr.second) {
        curr.first = curr.second - 1;
    }
    _range.get_subscriber().on_next(curr);
    emit rangeChanged();
}

QString EqualizerModel::minFrequencyReadout() const {
    const auto f = _rangeTable.at(_range.get_value().first);
    if (f > 3500.0) {
        return QString::number(f/1000.0, 'f', 0) + " kHz";
    } else if (f > 900.0) {
        return QString::number(f/1000.0, 'f', 1) + " kHz";
    } else {
        return QString::number(f, 'f', 0) + " Hz";
    }
}

QString EqualizerModel::maxFrequencyReadout() const {
    const auto f = _rangeTable.at(_range.get_value().second);
    if (f > 3500.0) {
        return QString::number(f/1000.0, 'f', 0) + " kHz";
    } else if (f > 900.0) {
        return QString::number(f/1000.0, 'f', 1) + " kHz";
    } else {
        return QString::number(f, 'f', 0) + " Hz";
    }
}

QObjectList EqualizerModel::filters() const {
    return _filters;
}

void EqualizerModel::setFilterSumSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _sumSeries = static_cast<QtCharts::QXYSeries*>(series);
        computeSumResponse();
        emit rangeChanged();
    }
}

void EqualizerModel::setTargetSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _targetSeries = static_cast<QtCharts::QXYSeries*>(series);
        computeSumResponse();
        emit rangeChanged();
    }
}

void EqualizerModel::setFilteredMeasurementSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _filteredSeries = static_cast<QtCharts::QXYSeries*>(series);
        computeSumResponse();
        emit rangeChanged();
    }
}

void EqualizerModel::addFilter(QtCharts::QAbstractSeries* response) {
    int f = 120;
    double g = -3.0;

    double q = 3.0;
    if (findMaxOvershoot(&f, &g)) {
        findQ(f, g, &q);
    }

    handles()->append(f, g);
    handles()->append(f - q, g / 2);
    handles()->append(f + q, g / 2);
    _filters.append(new FilterModel(f, q, g, response));
    emit filtersChanged();
    computeSumResponse();
}

void EqualizerModel::removeFilter(int index) {
    //handles()->remove(index);
    handles()->removePoints(index*3, 3);
    auto obj = _filters.at(index);
    _filters.removeAt(index);
    obj->deleteLater();
    emit filtersChanged();
    computeSumResponse();
}

void EqualizerModel::stepF(int index, int f) {
    auto filter = static_cast<FilterModel*>(_filters.at(index));
    filter->stepF(f);

    handles()->replace(index*3 + 0, filter->_f, filter->_g);
    handles()->replace(index*3 + 1, filter->_f - filter->_q, filter->_g / 2);
    handles()->replace(index*3 + 2, filter->_f + filter->_q, filter->_g / 2);

    computeSumResponse();
}

void EqualizerModel::stepQ(int index, double q) {
    auto filter = static_cast<FilterModel*>(_filters.at(index));
    filter->stepQ(q);

    handles()->replace(index*3 + 1, filter->_f - filter->_q, filter->_g / 2);
    handles()->replace(index*3 + 2, filter->_f + filter->_q, filter->_g / 2);

    computeSumResponse();
}

void EqualizerModel::stepG(int index, double g) {
    auto filter = static_cast<FilterModel*>(_filters.at(index));
    filter->stepG(g);

    handles()->replace(index*3 + 0, filter->_f, filter->_g);
    handles()->replace(index*3 + 1, filter->_f - filter->_q, filter->_g / 2);
    handles()->replace(index*3 + 2, filter->_f + filter->_q, filter->_g / 2);

    computeSumResponse();
}

void EqualizerModel::setLevel(int value) {
    _level.get_subscriber().on_next(value);
}

double EqualizerModel::yMin() const {
    return -48.0;
}

double EqualizerModel::yMax() const {
    return 12.0;
}

void EqualizerModel::moveHandle(int index, double x, double y) {
    if (index % 3 == 1) {
        return moveLeftQHandle(index, x);
    } else if (index % 3 == 2) {
        return moveRightQHandle(index, x);
    }

    auto filter = static_cast<FilterModel*>(_filters.at(index/3));
    int xIndex = std::round(x);
    double yIndex = std::round(y * 5.0) / 5.0;
    handles()->replace(index + 0, xIndex, yIndex);
    handles()->replace(index + 1, xIndex - filter->_q, yIndex / 2);
    handles()->replace(index + 2, xIndex + filter->_q, yIndex / 2);

    filter->moveHandle(xIndex, yIndex);
    computeSumResponse();
}

void EqualizerModel::moveLeftQHandle(int index, double x) {
    auto filter = static_cast<FilterModel*>(_filters.at(index/3));
    double xIndex = std::round(x * 2.0) / 2.0;

    auto q = abs(filter->_f - xIndex);
    filter->setQ(q);
    computeSumResponse();

    handles()->replace(index + 0, filter->_f - q, filter->_g / 2); //filter->_g / 2);
    handles()->replace(index + 1, filter->_f + q, filter->_g / 2); //filter->_g / 2);
}

void EqualizerModel::moveRightQHandle(int index, double x) {
    auto filter = static_cast<FilterModel*>(_filters.at(index/3));
    double xIndex = std::round(x * 2.0) / 2.0;

    auto q = abs(filter->_f - xIndex);
    filter->setQ(q);
    computeSumResponse();

    handles()->replace(index - 1, filter->_f - q, filter->_g / 2); //filter->_g / 2);
    handles()->replace(index + 0, filter->_f + q, filter->_g / 2); //filter->_g / 2);
}

void EqualizerModel::computeSumResponse() {
    if (!_sumSeries) return;

    std::vector<double> sum;
    sum.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        double fSum = 0.0;
        QObject* p;
        foreach (p, _filters) {
            auto filter = static_cast<FilterModel*>(p);
            auto response = static_cast<QtCharts::QXYSeries*>(filter->response());
            fSum += response->at(i).y();
        }
        sum.push_back(fSum);
    }
    _sum.get_subscriber().on_next(sum);

    _sumMax = -144.0;
    _sumMin = 144.0;
    for (const auto& s : sum) {
        _sumMax = std::max(_sumMax, s);
        _sumMin = std::min(_sumMin, s);
    }
    emit rangeChanged();
}

bool EqualizerModel::findMaxOvershoot(int* f, double* g) {
    if (_filteredSeries->count() == 0) return false;

    const int fRangeMin = _range.get_value().first * 8;
    const int fRangeMax = _range.get_value().second * 8;

    int fMax = 0;
    double gMax = 0.0;
    for (int i = fRangeMin; i < fRangeMax; ++i) {
        double g_ = std::round((_filteredSeries->at(i).y() - _targetSeries->at(i).y()) * 5.0) / 5.0;
        if (gMax < g_) {
            fMax = i;
            gMax = g_;
        }
    }

    if (fMax != 0 && gMax != 0.0) {
        *f = fMax;
        *g = -gMax;
        return true;
    }

    return false;
}

void EqualizerModel::findQ(int f, double g, double* q) {
    const int fRangeMin = _range.get_value().first * 8; //std::max(_range.get_value().first * 8, f - 8);
    const int fRangeMax = _range.get_value().second * 8; //std::min(_range.get_value().second * 8, f + 8);

    double minDeviation = std::numeric_limits<double>::infinity();
    for (int i = 1; i < 81; ++i) {
        const auto pow2N = pow(2, i/12.0);
        const auto q_ = sqrt(pow2N)/(pow2N - 1);

        AudioFilter lp(FilterType::Peak, _frequencyTable.at(f), g, q_);
        auto response = lp.response(_frequencyTable, 1);

        double sumDiff = 0.0;
        for (int j = fRangeMin; j < fRangeMax; ++j) {
            sumDiff += abs(_filteredSeries->at(j).y() - _targetSeries->at(j).y() + 20 * log10(abs(response.at(j))));
        }

        if (sumDiff < minDeviation) {
            minDeviation = sumDiff;
        } else {
            *q = std::max(i - 1, 1);
            return;
        }

        if (i == 80) {
            *q = 80;
            return;
        }
    }
}
