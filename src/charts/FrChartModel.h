#pragma once

#include "ChartModel.h"

class FrChartModel : public ChartModel {
    Q_OBJECT
    Q_PROPERTY(QStringList calibrations READ calibrations CONSTANT)

public:
    explicit FrChartModel(QObject *parent = nullptr);

    QStringList calibrations() const;
    Q_INVOKABLE void setCalibration(int index);

    Q_INVOKABLE void setLogSeries(QtCharts::QAbstractSeries* series);
    Q_INVOKABLE void setCalibrationSeries(QtCharts::QAbstractSeries* series);

signals:

private:
    void updateChart();
    void onCurrentMeasurementChangedF(Project* measurement);

    QtCharts::QXYSeries* _logSeries = nullptr;
    Project* _project = nullptr;

    // Calibration members
    std::map<Calibration, QVector<QPointF>> _calibrations;
    QtCharts::QXYSeries* _calibrationSeries = nullptr;
};
