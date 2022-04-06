#include "EqualizerModel.h"

#include <utility>

#include <QAudioDeviceInfo>
#include <QDebug>
#include <QSaveFile>
#include <QTimer>
#include <QUrl>
#include <QVariant>

#include "AudioFilter.h"
#include "FilterModel.h"
#include "FrequencyTable.h"

// https://www.sonarworks.com/soundid-reference/blog/learn/eq-curves-defined/

EqualizerModel::EqualizerModel(QObject *parent)
    : ChartModel(parent) {
    setType(FrequencyResponse);
    FrequencyTable<double> table;
    _frequencyTable = table.frequencies();
}

QObjectList EqualizerModel::filters() const {
    return _filters;
}

void EqualizerModel::setSumSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _sumSeries = static_cast<QtCharts::QXYSeries*>(series);
        computeSumResponse();
        emit rangeChanged();
    }
}

void EqualizerModel::addFilter(QtCharts::QAbstractSeries* response) {
    int f = 120;
    double g = -3.0;
    handles()->append(f, g);
    handles()->append(f - 3, g/2);
    handles()->append(f + 3, g/2);
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

double EqualizerModel::yMin() const {
    return -18.0;
}

double EqualizerModel::yMax() const {
    return 6.0;
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

    handles()->replace(index + 0, filter->_f - q, filter->_g / 2);
    handles()->replace(index + 1, filter->_f + q, filter->_g / 2);

    filter->setQ(q);
    computeSumResponse();
}

void EqualizerModel::moveRightQHandle(int index, double x) {
    auto filter = static_cast<FilterModel*>(_filters.at(index/3));
    double xIndex = std::round(x * 2.0) / 2.0;

    auto q = abs(filter->_f - xIndex);

    handles()->replace(index - 1, filter->_f - q, filter->_g / 2);
    handles()->replace(index + 0, filter->_f + q, filter->_g / 2);

    filter->setQ(q);
    computeSumResponse();
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

    _sumMax = -144.0;
    _sumMin = 144.0;
    for (const auto& s : sum) {
        _sumMax = std::max(_sumMax, s);
        _sumMin = std::min(_sumMin, s);
    }
    emit rangeChanged();

    QVector<QPointF> points;
    points.reserve(_frequencyTable.size());
    for (int i = 0; i < _frequencyTable.size(); ++i) {
        points.append( { _frequencyTable.at(i), sum.at(i) } );
    }
    _sumSeries->replace(points);
}
