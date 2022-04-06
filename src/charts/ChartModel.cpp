#include "ChartModel.h"

#include <QCoreApplication>
#include <QPointF>

#include <FrequencyTable.h>
#include <measure/Measurement.h>
#include <measure/MeasurementManager.h>
#include <status/StatusModel.h>

#include "LinChart.h"
#include "LogChart.h"
#include "MaterialDesignIcon.h"
#include "TimeTable.h"

ChartModel::ChartModel(QObject *parent)
    : QObject{parent} {
    _charts[ImpulseResponse] = std::make_unique<LinChart>();
    _charts[FrequencyResponse] = std::make_unique<LogChart>();
}

void ChartModel::zoom(double zoom) {
    _charts[_type]->zoom(zoom);
    emit rangeChanged();
}

void ChartModel::pan(double pan) {
    _charts[_type]->pan(pan);
    emit rangeChanged();
}

void ChartModel::setType(Type type) {
    _type = type;
    emit rangeChanged();
}

ChartModel::Type ChartModel::type() const {
    return _type;
}

void ChartModel::setHandles(QtCharts::QAbstractSeries* series) {
    if (series) {
        _handles = static_cast<QtCharts::QXYSeries*>(series);
    }
}

void ChartModel::moveHandle(int index, double x, double y) {
}

double ChartModel::xMin() const {
    return _charts.at(_type)->_xMin;
}

double ChartModel::xMax() const {
    return _charts.at(_type)->_xMax;
}

double ChartModel::yMin() const {
    return _charts.at(_type)->_yMin;
}

double ChartModel::yMax() const {
    return _charts.at(_type)->_yMax;
}

int ChartModel::tickInterval() const {
    return _charts.at(_type)->_tickInterval;
}

QtCharts::QXYSeries* ChartModel::handles() {
    return _handles;
}
