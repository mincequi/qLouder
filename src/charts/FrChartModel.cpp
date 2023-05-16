#include "FrChartModel.h"

#include <common/Config.h>
#include <project/ProjectManager.h>

FrChartModel::FrChartModel(QObject *parent)
    : ChartModel{parent} {
    setType(FrequencyResponse);
    connect(&ProjectManager::instance(), &ProjectManager::currentProjectChanged,
            this, &FrChartModel::onCurrentMeasurementChangedF);
}

QStringList FrChartModel::calibrations() const {
    return { "None", "0°", "90°" };
}

void FrChartModel::setCalibration(int index) {
    cfg->calibration = static_cast<Calibration>(index);
    if (_calibrationSeries)
        _calibrationSeries->replace(_calibrations[cfg->calibration]);

    ProjectManager::instance().setFrCalibration(cfg->calibration);
}

void FrChartModel::setLogSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _logSeries = static_cast<QtCharts::QXYSeries*>(series);
    }
}

void FrChartModel::setCalibrationSeries(QtCharts::QAbstractSeries* series) {
    if (series) {
        _calibrationSeries = static_cast<QtCharts::QXYSeries*>(series);
    }
}

void FrChartModel::updateChart() {
    if (!_project) {
        return;
    }

    const auto& fs = _project->frequencies();
    const auto& fir = _project->calibratedFr();

    QVector<QPointF> points;
    points.reserve(fs.size());
    for (int i = 0; i < fs.size(); ++i) {
        points.append( { fs.at(i), fir.at(i) } );
    }
    _logSeries->replace(points);

}

void FrChartModel::onCurrentMeasurementChangedF(Project* project) {
    _project = project;
    if (!_project) {
        _logSeries->clear();
        _calibrationSeries->clear();
        _calibrations.clear();
        return;
    }

    // Install callback to get notified, when measurement is changed
    _project->setFrChangedCallback(std::bind(&FrChartModel::updateChart, this));

    // Calibrations are static, so we update our series
    _calibrations.clear();
    const auto& fs = project->frequencies();
    const auto& cFr = _project->calibratedFr();
    const auto& c0 = project->frCalibration(Calibration0);
    const auto& c90 = project->frCalibration(Calibration90);

    auto& points0 = _calibrations[Calibration0];
    points0.reserve(fs.size());
    for (int i = 0; i < c0.size(); ++i) {
        points0.append( { fs.at(i), c0.at(i) } );
    }
    auto& points1 = _calibrations[Calibration90];
    points1.reserve(fs.size());
    for (int i = 0; i < c90.size(); ++i) {
        points1.append( { fs.at(i), c90.at(i) } );
    }
    _calibrationSeries->replace(_calibrations[cfg->calibration]);

    // Initial chart update
    updateChart();
}
