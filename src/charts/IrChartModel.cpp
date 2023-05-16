#include "IrChartModel.h"

#include "TimeTable.h"
#include <project/ProjectManager.h>

#include <cmath>
#include <QCoreApplication>

IrChartModel::IrChartModel(QObject *parent)
    : ChartModel{parent} {
    connect(&ProjectManager::instance(), &ProjectManager::currentProjectChanged,
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

bool IrChartModel::hasMeasurement() const {
    return _project != nullptr;
}

void IrChartModel::moveIrWindowLeft(int step) {
    const auto val = fabs(_project->irWindowLeft());

    if (step < 0)
        _project->setIrWindowLeft(-TimeTable::lower(val));
    else
        _project->setIrWindowLeft(-TimeTable::upper(val));

    updateChart();
}

void IrChartModel::moveIrWindowRight(int step) {
    const auto val = fabs(_project->irWindowRight());

    if (step < 0)
        _project->setIrWindowRight(TimeTable::lower(val));
    else
        _project->setIrWindowRight(TimeTable::upper(val));

    updateChart();
}

void IrChartModel::moveHandle(int index, double x, double y) {
    if (index == 0) {
        x = std::min(x, -0.1);
        _project->setIrWindowLeft(x);
    } else if (index == 1) {
        x = std::max(x, +0.5);
        _project->setIrWindowRight(x);
    }

    updateChart();
}

QString IrChartModel::irWindowLeftReadout() const {
    if (!_project) {
        return {};
    }

    auto val = fabs(_project->irWindowLeft());
    if (val < 10.0)
         return QString::number(val, 'f', 1) + " ms";
    else
        return QString::number(val, 'f', 0) + " ms";
}

QString IrChartModel::irWindowRightReadout() const {
    if (!_project) {
        return {};
    }

    auto val = _project->irWindowRight();
    if (val < 10.0)
         return QString::number(val, 'f', 1) + " ms";
    else
        return QString::number(val, 'f', 0) + " ms";
}

double IrChartModel::irWindowLeft() const {
    if (!_project) {
        return -100.0; //std::numeric_limits<double>::quiet_NaN();
    }

    return _project->irWindowLeft();
}

double IrChartModel::irWindowRight() const {
    if (!_project) {
        return 400.0; //std::numeric_limits<double>::quiet_NaN();
    }

    return _project->irWindowRight();
}

void IrChartModel::updateChart() {
    if (!_project || !_series) {
        return;
    }

    // Update UI controls
    handles()->replace(0, _project->irWindowLeft(), 0.0);
    handles()->replace(1, _project->irWindowRight(), 0.0);

    // Process core events, before doing costly operations
    QCoreApplication::processEvents();

    const auto& wir = _project->windowedIr();
    QVector<QPointF> points;
    points.reserve(wir.size());
    const double durationMs = (_project->irWindowRight() - _project->irWindowLeft());
    for (int i = 0; i < wir.size(); ++i) {
        points.append( { _project->irWindowLeft() + (double)i/wir.size()*durationMs, wir.at(i) } );
    }
    updateIrWindow();

    _upperEnvelopeSeries->replace(simplify(points));
    /*
        _lowerEnvelopeSeries->replace(lowerEnvelope);
        */
}

void IrChartModel::updateIrWindow() {
    const auto& wir = _project->windowedIr();
    const auto& w = _project->irWindow();

    QVector<QPointF> points;
    points.reserve(w.size());
    const double durationMs = (_project->irWindowRight() - _project->irWindowLeft());
    points.append( { _project->irWindowLeft(), w.front() } );
    for (int i = 1; i < w.size()-1; ++i) {
        if (fabs(points.last().ry() - w.at(i)) > 0.005)
            points.append( { _project->irWindowLeft() + (double)i/w.size()*durationMs, w.at(i) } );
    }
    points.append( { _project->irWindowRight(), w.back() } );

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

void IrChartModel::onCurrentMeasurementChangedF(Project* measurement) {
    _project = measurement;
    emit measurementChanged();
    if (!_project || !_series) {
        return;
    }

    updateChart();
    emit irWindowChanged();
}
