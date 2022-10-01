import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import QtCharts 2.3

import ChartModel 1.0
import FrChartModel 1.0

Chart {
    id: chart
    type: ChartModel.FrequencyResponse

    property var calibrationSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "Calibration", chart.xAxisLog, chart.yAxis)
    property var logSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "Frequency response", chart.xAxisLog, chart.yAxis)

    Component.onCompleted: {
        logSeries.color = Material.accentColor
        logSeries.width = 1.0
        model.setLogSeries(logSeries)

        calibrationSeries.color = chart.foregroundColor
        calibrationSeries.width = 1.0
        model.setCalibrationSeries(calibrationSeries)
    }
}
