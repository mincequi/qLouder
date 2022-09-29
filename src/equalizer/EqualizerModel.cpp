#include "EqualizerModel.h"

#include <chrono>
#include <rxcpp/operators/rx-combine_latest.hpp>

#include "AudioFilter.h"
#include "FilterModel.h"
#include <measure/MeasurementManager.h>
#include <status/StatusModel.h>
#include <target/TargetModel.h>

// https://www.sonarworks.com/soundid-reference/blog/learn/eq-curves-defined/

EqualizerModel::EqualizerModel(const TargetModel& targetModel,
                               QObject *parent)
    : ChartModel(parent),
      _targetModel(targetModel),
      _range({3, 28}) {
    ChartModel::setType(FrequencyResponse);
    FrequencyTable<double> table;
    _frequencyTable = table.frequencies();
    FrequencyTable<double> rangeTable(3);
    _rangeTable = rangeTable.frequencies();

    MeasurementManager::instance().calibratedFr()
            .combine_latest([](const auto& measurement, const auto& filter) {
        std::vector<double> out;
        out.reserve(measurement.size());
        for (size_t i = 0; i < measurement.size(); ++i) {
            out.push_back(measurement.at(i) + filter.at(i));
        }
        return out;
    }, _filterSum.get_observable())
            .subscribe([this](const auto& filteredMeasurement) {
        QVector<QPointF> points;
        points.reserve(_frequencyTable.size());
        for (int i = 0; i < _frequencyTable.size(); ++i) {
            points.append( { _frequencyTable.at(i), filteredMeasurement.at(i) } );
        }
        _filteredResponseSeries->replace(points);
    });

    _filterSum.get_observable()
            .subscribe([this](const auto& sum) {
        QVector<QPointF> points;
        points.reserve(_frequencyTable.size());
        for (int i = 0; i < _frequencyTable.size(); ++i) {
            points.append( { _frequencyTable.at(i), sum.at(i) } );
        }
        _filterSumSeries->replace(points);
    });

    _targetModel.fr()
            .combine_latest([](const auto& sum, auto level) {
        std::vector<double> out;
        out.reserve(sum.size());
        for (size_t i = 0; i < sum.size(); ++i) {
            out.push_back(sum.at(i) + level);
        }
        return out;
    }, _level.get_observable())
            .subscribe([this](const auto& sum) {
        QVector<QPointF> points;
        points.reserve(_frequencyTable.size());
        for (int i = 0; i < _frequencyTable.size(); ++i) {
            points.append( { _frequencyTable.at(i), sum.at(i) } );
        }
        _targetSeries->replace(points);
    });
}

const QStringList& EqualizerModel::types() {
    static const QStringList _types = { "None", "Peaking", "Low Pass", "High Pass", "Low Shelf", "High Shelf" };
    return _types;
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
        _filterSumSeries = static_cast<QtCharts::QXYSeries*>(series);
        computeFilterSum();
        emit rangeChanged();
    }
}

void EqualizerModel::setTargetSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _targetSeries = static_cast<QtCharts::QXYSeries*>(series);
        computeFilterSum();
        emit rangeChanged();
    }
}

void EqualizerModel::setFilteredMeasurementSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _filteredResponseSeries = static_cast<QtCharts::QXYSeries*>(series);
        computeFilterSum();
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
    _filters.append(new FilterModel(*this, FilterType::Peak, f, q, g, response));
    emit filtersChanged();
    computeFilterSum();
}

void EqualizerModel::removeFilter(int index) {
    //handles()->remove(index);
    handles()->removePoints(index*3, 3);
    auto obj = _filters.at(index);
    _filters.removeAt(index);
    obj->deleteLater();
    emit filtersChanged();
    computeFilterSum();
}

void EqualizerModel::setType(int index, int type) {
    auto filter = static_cast<FilterModel*>(_filters.at(index));
    filter->setType(static_cast<FilterType>(type));

    computeFilterSum();
}

void EqualizerModel::stepF(int index, int f) {
    auto filter = static_cast<FilterModel*>(_filters.at(index));
    filter->stepF(f);

    computeFilterSum();
}

void EqualizerModel::stepQ(int index, double q) {
    auto filter = static_cast<FilterModel*>(_filters.at(index));
    filter->stepQ(q);

    computeFilterSum();
}

void EqualizerModel::stepG(int index, double g) {
    auto filter = static_cast<FilterModel*>(_filters.at(index));
    filter->stepG(g);

    computeFilterSum();
}

void EqualizerModel::setLevel(int value) {
    _level.get_subscriber().on_next(value);
}

double EqualizerModel::yMin() const {
    return -36.0;
}

double EqualizerModel::yMax() const {
    return 12.0;
}

void EqualizerModel::moveHandle(int index, double x, double y) {
    if (index % 3 == 1) {
        return moveLeftHandle(index, x);
    } else if (index % 3 == 2) {
        return moveRightHandle(index, x);
    }

    auto filter = static_cast<FilterModel*>(_filters.at(index/3));
    int xIndex = std::round(x);
    double yIndex = std::round(y * 5.0) / 5.0;

    filter->onMainHandleMoved(xIndex, yIndex);
    computeFilterSum();
}

void EqualizerModel::moveLeftHandle(int index, double x) {
    auto filter = static_cast<FilterModel*>(_filters.at(index/3));
    double xIndex = std::round(x * 2.0) / 2.0;

    filter->onLeftHandleMoved(xIndex);
    computeFilterSum();
}

void EqualizerModel::moveRightHandle(int index, double x) {
    auto filter = static_cast<FilterModel*>(_filters.at(index/3));
    double xIndex = std::round(x * 2.0) / 2.0;

    filter->onRightHandleMoved(xIndex);
    computeFilterSum();
}

void EqualizerModel::computeFilterSum() {
    if (!_filterSumSeries) return;

    std::vector<double> sum;
    sum.reserve(_frequencyTable.size());
    for (size_t i = 0; i < _frequencyTable.size(); ++i) {
        double fSum = 0.0;
        QObject* p;
        foreach (p, _filters) {
            auto filter = static_cast<FilterModel*>(p);
            auto response = static_cast<QtCharts::QXYSeries*>(filter->response());
            fSum += response->at(i).y();
        }
        sum.push_back(fSum);
    }
    _filterSum.get_subscriber().on_next(sum);

    _sumMax = -144.0;
    _sumMin = 144.0;
    for (const auto& s : sum) {
        _sumMax = std::max(_sumMax, s);
        _sumMin = std::min(_sumMin, s);
    }
    emit rangeChanged();
}

bool EqualizerModel::findMaxOvershoot(int* f, double* g) {
    if (_filteredResponseSeries->count() == 0) return false;

    const int fRangeMin = _range.get_value().first * 8;
    const int fRangeMax = _range.get_value().second * 8;

    int fMax = 0;
    double gMax = 0.0;
    for (int i = fRangeMin; i < fRangeMax; ++i) {
        double g_ = std::round((_filteredResponseSeries->at(i).y() - _targetSeries->at(i).y()) * 5.0) / 5.0;
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
    double minDeviation = std::numeric_limits<double>::infinity();
    for (int i = 1; i < 81; ++i) {
        auto sumDiff = deviationWithFilter(f, g, i);

        if (sumDiff <= minDeviation) {
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

double EqualizerModel::deviationWithFilter(int f, double g, int q) {
    const int fRangeMin = _range.get_value().first * 8;
    const int fRangeMax = _range.get_value().second * 8;
    const auto pow2N = pow(2, q/12.0);
    const auto q_ = sqrt(pow2N)/(pow2N - 1);

    AudioFilter lp(FilterType::Peak, _frequencyTable.at(f), g, q_);
    auto response = lp.response(_frequencyTable, 1);

    double sumDiff = 0.0;
    for (int j = fRangeMin; j < fRangeMax; ++j) {
        sumDiff += abs(_filteredResponseSeries->at(j).y() - _targetSeries->at(j).y() + 20 * log10(abs(response.at(j))));
    }

    return sumDiff;
}

double EqualizerModel::deviation() {
    const int fRangeMin = _range.get_value().first * 8;
    const int fRangeMax = _range.get_value().second * 8;

    double sumDiff = 0.0;
    for (int j = fRangeMin; j < fRangeMax; ++j) {
        sumDiff += abs(_filteredResponseSeries->at(j).y() - _targetSeries->at(j).y());
    }

    return sumDiff;
}

void EqualizerModel::optimize() {
    const auto t1 = std::chrono::high_resolution_clock::now();

    for (int index = 0; index < _filters.size(); ++index) {
        auto filter = static_cast<FilterModel*>(_filters.at(index));

        double minDeviation = std::numeric_limits<double>::infinity();
        const int minQ = std::max(1, (int)filter->_qIndex - 2);
        const int maxQ = std::min(80, (int)filter->_qIndex + 2);
        const int minF = std::max(8, (int)filter->_fIndex - 2);
        const int maxF = std::min(248, (int)filter->_fIndex + 2);
        const double minG = filter->_g - 0.4;
        const double maxG = filter->_g + 0.401;

        int f_ = 0;
        int q_ = 0;
        double g_ = 0.0;
        for (int f = minF; f <= maxF; ++f) {
            for (int q = minQ; q <= maxQ; ++q) {
                for (double g = minG; g <= maxG; g += 0.2) {

                    filter->_fIndex = f;
                    filter->_qIndex = q;
                    filter->_g = g;
                    filter->computeResponse();
                    computeFilterSum();

                    auto sumDiff = deviation();
                    if (sumDiff < minDeviation) {
                        minDeviation = sumDiff;
                        f_ = f;
                        q_ = q;
                        g_ = g;
                    }
                }
            }
        }

        filter->_fIndex = f_;
        filter->_qIndex = q_;
        filter->_g = g_;
        filter->computeResponse();
        emit filter->valuesChanged();

        handles()->replace(index*3 + 0, filter->_fIndex, filter->_g);
        handles()->replace(index*3 + 1, filter->_fIndex - filter->_qIndex, filter->_g / 2);
        handles()->replace(index*3 + 2, filter->_fIndex + filter->_qIndex, filter->_g / 2);
    }
    const auto t2 = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    StatusModel::instance()->showMessage("Optimize took " + QString::number(ms.count()) + " ms", 3000);

    emit filtersChanged();
    computeFilterSum();
}
