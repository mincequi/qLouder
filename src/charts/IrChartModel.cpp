#include "IrChartModel.h"

#include "TimeTable.h"
#include "measure/MeasurementManager.h"

#include <QCoreApplication>

IrChartModel::IrChartModel(QObject *parent)
    : ChartModel{parent} {
    connect(&MeasurementManager::instance(), &MeasurementManager::currentMeasurementChangedF,
            this, &IrChartModel::onCurrentMeasurementChangedF);

}

void IrChartModel::setSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _series = static_cast<QtCharts::QXYSeries*>(series);
    }
}

void IrChartModel::setUpperEnvelopeSeries(QtCharts::QAbstractSeries *series) {
    if (series) {
        _upperEnvelopeSeries = static_cast<QtCharts::QXYSeries*>(series);
    }
}

void IrChartModel::setLowerEnvelopeSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _lowerEnvelopeSeries = static_cast<QtCharts::QXYSeries*>(series);
    }
}

void IrChartModel::setIrWindowSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _irWindowSeries = static_cast<QtCharts::QXYSeries*>(series);
    }
}

void IrChartModel::setIrWindowHandles(QtCharts::QAbstractSeries* series) {
    if (series) {
        _irWindowHandles = static_cast<QtCharts::QXYSeries*>(series);
        //auto arrow = material::pixmap("arrow-split-vertical", QSize(24, 24));
        //auto arrow = material::pixmap("arrow-collapse-left", QSize(24, 24));
        //_irWindowHandles->setBrush(arrow);
    }
}

bool IrChartModel::hasMeasurement() const {
    return _measurement != nullptr;
}

void IrChartModel::moveIrWindowLeft(int step) {
    const auto val = fabs(_measurement->irWindowLeft());

    if (step < 0)
        _measurement->setIrWindowLeft(-TimeTable::lower(val));
    else
        _measurement->setIrWindowLeft(-TimeTable::upper(val));

    updateChart();
}

void IrChartModel::moveIrWindowRight(int step) {
    const auto val = fabs(_measurement->irWindowRight());

    if (step < 0)
        _measurement->setIrWindowRight(TimeTable::lower(val));
    else
        _measurement->setIrWindowRight(TimeTable::upper(val));

    updateChart();
}

void IrChartModel::moveHandle(int index, double x, double y) {
    if (index == 0) {
        x = std::min(x, -0.1);
        _measurement->setIrWindowLeft(x);
    } else if (index == 1) {
        x = std::max(x, +0.5);
        _measurement->setIrWindowRight(x);
    }

    updateChart();
}

QString IrChartModel::irWindowLeftReadout() const {
    if (!_measurement) {
        return {};
    }

    auto val = fabs(_measurement->irWindowLeft());
    if (val < 10.0)
         return QString::number(val, 'f', 1) + " ms";
    else
        return QString::number(val, 'f', 0) + " ms";
}

QString IrChartModel::irWindowRightReadout() const {
    if (!_measurement) {
        return {};
    }

    auto val = _measurement->irWindowRight();
    if (val < 10.0)
         return QString::number(val, 'f', 1) + " ms";
    else
        return QString::number(val, 'f', 0) + " ms";
}

double IrChartModel::irWindowLeft() const {
    if (!_measurement) {
        return -100.0; //std::numeric_limits<double>::quiet_NaN();
    }

    return _measurement->irWindowLeft();
}

double IrChartModel::irWindowRight() const {
    if (!_measurement) {
        return 400.0; //std::numeric_limits<double>::quiet_NaN();
    }

    return _measurement->irWindowRight();
}

void IrChartModel::updateChart() {
    if (!_measurement || !_series) {
        return;
    }

    // Update UI controls
    _irWindowHandles->replace(0, _measurement->irWindowLeft(), 0.0);
    _irWindowHandles->replace(1, _measurement->irWindowRight(), 0.0);

    // Process core events, before doing costly operations
    QCoreApplication::processEvents();

    const auto& wir = _measurement->windowedIr();
    QVector<QPointF> points;
    points.reserve(wir.size());
    const double durationMs = (_measurement->irWindowRight() - _measurement->irWindowLeft());
    for (int i = 0; i < wir.size(); ++i) {
        points.append( { _measurement->irWindowLeft() + (double)i/wir.size()*durationMs, wir.at(i) } );
    }
    updateIrWindow();

    _upperEnvelopeSeries->replace(simplify(points));
    /*
        _lowerEnvelopeSeries->replace(lowerEnvelope);
        */
}

void IrChartModel::updateIrWindow() {
    const auto& wir = _measurement->windowedIr();
    const auto& w = _measurement->irWindow();

    QVector<QPointF> points;
    points.reserve(w.size());
    const double durationMs = (_measurement->irWindowRight() - _measurement->irWindowLeft());
    points.append( { _measurement->irWindowLeft(), w.front() } );
    for (int i = 1; i < w.size()-1; ++i) {
        if (fabs(points.last().ry() - w.at(i)) > 0.005)
            points.append( { _measurement->irWindowLeft() + (double)i/w.size()*durationMs, w.at(i) } );
    }
    points.append( { _measurement->irWindowRight(), w.back() } );

    _irWindowSeries->replace(points);
    emit irWindowChanged();
}

QVector<QPointF> IrChartModel::simplify(const QVector<QPointF>& in) {
    QVector<QPointF> out;
    out.append(in.first());
    for (int i = 1; i < in.size()-1; ++i) {
        auto diff = fabs(out.last().y() - in.at(i).y());
        if (diff > 0.01)
            out.append(in.at(i));
    }
    out.append(in.last());
    return out;
}

void IrChartModel::onCurrentMeasurementChangedF(Measurement<float>* measurement) {
    _measurement = measurement;
    emit measurementChanged();
    if (!_measurement || !_series) {
        return;
    }

    updateChart();
    emit irWindowChanged();
}
