import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import QtCharts 2.0

import ".."

import ChartModel 1.0
import FrChartModel 1.0

Chart {
    id: chart
    type: ChartModel.FrequencyResponse

    property var calibrationSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "Calibration", chart.xAxisLog, chart.yAxis)
    property var logSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "Frequency response", chart.xAxisLog, chart.yAxis)

    model: FrChartModel {
        id: model
        type: chart.type
    }

    toolBarChildren: [
        RowLayout {
            spacing: 0
            Layout.topMargin: 6

            SmallToolButton {
                iconName: "magnify-minus"
                flat: true
                onClicked: model.zoom(0.5);
            }
            SmallToolButton {
                id: zoomIn
                iconName: "magnify-plus"
                flat: true
                onClicked: model.zoom(2.0);
            }
            SmallToolButton {
                iconName: "chevron-left"
                flat: true
                onClicked: model.pan(-0.2);
            }
            SmallToolButton {
                iconName: "chevron-right"
                flat: true
                onClicked: model.pan(0.2);
            }
        },

        ToolSeparator {
            anchors.left: parent.left
            anchors.right: parent.right
            topPadding: 6
            bottomPadding: 6
            orientation: Qt.Horizontal
        },

        Label {
            height: 24
            text: "Calibration"
            leftPadding: 3
            font.pointSize: 12
        },
        SmallComboBox {
            flat: true
            implicitWidth: parent.width
            model: model.calibrations
            onCurrentIndexChanged: model.setCalibration(currentIndex)
        },

        ToolSeparator {
            anchors.left: parent.left
            anchors.right: parent.right
            topPadding: 6
            bottomPadding: 6
            orientation: Qt.Horizontal
        },

        // spacer item
        Item {
            Layout.fillHeight: true
        }
    ]

    Component.onCompleted: {
        logSeries.color = Material.accentColor
        logSeries.width = 1.0
        model.setLogSeries(logSeries)

        calibrationSeries.color = chart.foregroundColor
        calibrationSeries.width = 1.0
        model.setCalibrationSeries(calibrationSeries)
    }
}
