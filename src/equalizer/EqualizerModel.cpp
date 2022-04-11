#include "EqualizerModel.h"

#include "FilterModel.h"
#include <measure/MeasurementManager.h>
#include <target/TargetModel.h>

#include <rxcpp/operators/rx-combine_latest.hpp>

// https://www.sonarworks.com/soundid-reference/blog/learn/eq-curves-defined/

EqualizerModel::EqualizerModel(const TargetModel& targetModel,
                               QObject *parent)
    : ChartModel(parent),
      _targetModel(targetModel) {
    setType(FrequencyResponse);
    FrequencyTable<double> table;
    _frequencyTable = table.frequencies();
    FrequencyTable<double> rangeTable(3);
    _rangeTable = rangeTable.frequencies();
    _maxFrequencySlider = _rangeTable.size()-3;

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
        QVector<QPointF> points;
        points.reserve(_frequencyTable.size());
        for (int i = 0; i < _frequencyTable.size(); ++i) {
            points.append( { _frequencyTable.at(i), sum.at(i) } );
        }
        _targetSeries->replace(points);
    });
}

double EqualizerModel::minFrequencySlider() const {
    return _minFrequencySlider;
}

double EqualizerModel::maxFrequencySlider() const {
    return _maxFrequencySlider;
}

void EqualizerModel::setMinFrequencySlider(double value) {
    _minFrequencySlider = std::min(value, (double)(_rangeTable.size()-2));
    if (_maxFrequencySlider <= _minFrequencySlider) {
        _maxFrequencySlider = _minFrequencySlider + 1;
    }
    emit rangeChanged();
}

void EqualizerModel::setMaxFrequencySlider(double value) {
    _maxFrequencySlider = std::max(value, 1.0);
    if (_minFrequencySlider >= _maxFrequencySlider) {
        _minFrequencySlider = _maxFrequencySlider - 1;
    }
    emit rangeChanged();
}

QString EqualizerModel::minFrequencyReadout() const {
    const auto f = _rangeTable.at(_minFrequencySlider);
    if (f > 3500.0) {
        return QString::number(f/1000.0, 'f', 0) + " kHz";
    } else if (f > 900.0) {
        return QString::number(f/1000.0, 'f', 1) + " kHz";
    } else {
        return QString::number(f, 'f', 0) + " Hz";
    }
}

QString EqualizerModel::maxFrequencyReadout() const {
    const auto f = _rangeTable.at(_maxFrequencySlider);
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
    handles()->append(f, g);
    handles()->append(f - 3, g / 2);
    handles()->append(f + 3, g / 2);
    _filters.append(new FilterModel(response));
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
