import QtQuick 2.12
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtCharts 2.3

import CrossoverModel 1.0

Page {
    header: ToolBar {
        height: 24
        width: parent.width
        Material.elevation: 0
        leftPadding: 0
        spacing: 3

        // Tool bar
        RowLayout {
            anchors.left: parent.left
            width: parent.width
            layoutDirection: "LeftToRight"
            spacing: 0
        }
    }

    ColumnLayout {
        anchors.topMargin: 1
        anchors.fill: parent

        FrChart {
            id: chart
            model: CrossoverModel
            toolBarChildren: [
                Label {
                    height: 24
                    padding: 6
                    text: "Low pass"
                    font.pixelSize: 12
                },
                UpDownSpinBox {
                    label: "F"
                    value: CrossoverModel.lowPassFrequencyReadout
                    unit: CrossoverModel.lowPassFrequencyUnitReadout
                    onValueUp: CrossoverModel.stepParam(0, +1, 0)
                    onValueDown: CrossoverModel.stepParam(0, -1, 0)
                },
                UpDownSpinBox {
                    label: "Q"
                    value: CrossoverModel.lowPassQ.toPrecision(3)
                    onValueUp: CrossoverModel.stepParam(0, 0, +0.2)
                    onValueDown: CrossoverModel.stepParam(0, 0, -0.2)
                },
                UpDownSpinBox {
                    label: "G"
                    value: CrossoverModel.lowPassG.toFixed(1)
                    unit: "dB"
                    onValueUp: CrossoverModel.stepParam(2, 0, +0.2)
                    onValueDown: CrossoverModel.stepParam(2, 0, -0.2)
                },
                SmallComboBox {
                    flat: true
                    implicitWidth: 96
                    model: [ "Order 2", "Order 4" ]
                    onCurrentIndexChanged: CrossoverModel.setOrder(0, currentIndex)
                },

                ToolSeparator {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    topPadding: 0
                    bottomPadding: 0
                    orientation: Qt.Horizontal
                },

                Label {
                    height: 24
                    padding: 6
                    text: "High pass"
                    font.pixelSize: 12
                },
                UpDownSpinBox {
                    label: "F"
                    value: CrossoverModel.highPassFrequencyReadout
                    unit: CrossoverModel.highPassFrequencyUnitReadout
                    onValueUp: CrossoverModel.stepParam(1, +1, 0)
                    onValueDown: CrossoverModel.stepParam(1, -1, 0)
                },
                UpDownSpinBox {
                    label: "Q"
                    value: CrossoverModel.highPassQ.toPrecision(3)
                    onValueUp: CrossoverModel.stepParam(1, 0, +0.2)
                    onValueDown: CrossoverModel.stepParam(1, 0, -0.2)
                },
                UpDownSpinBox {
                    label: "G"
                    value: CrossoverModel.highPassG.toFixed(1)
                    unit: "dB"
                    onValueUp: CrossoverModel.stepParam(3, 0, +0.2)
                    onValueDown: CrossoverModel.stepParam(3, 0, -0.2)
                },
                SmallComboBox {
                    flat: true
                    implicitWidth: 96
                    model: [ "Order 2", "Order 4" ]
                    onCurrentIndexChanged: CrossoverModel.setOrder(1, currentIndex)
                },
                SmallSwitch {
                    implicitHeight: 24
                    implicitWidth: 96
                    onCheckedChanged: CrossoverModel.invert(checked)
                },

                // spacer item
                Item {
                    Layout.fillHeight: true
                },

                Rectangle {
                    width: 96
                    height: 1
                    color: Material.background
                },

                Label {
                    height: 24
                    leftPadding: 6
                    topPadding: 6
                    bottomPadding: 6
                    text: "Deviation"
                    font.bold: true
                    font.pixelSize: 12
                },

                UpDownSpinBox {
                    label: "Min"
                    value: CrossoverModel.sumMin.toFixed(1)
                    unit: "dB"
                    valueColor: Material.accent
                    buttonsVisible: false
                },
                UpDownSpinBox {
                    label: "Max"
                    value: CrossoverModel.sumMax.toFixed(1)
                    unit: "dB"
                    valueColor: Material.accent
                    buttonsVisible: false
                },
                UpDownSpinBox {
                    label: "Ripple"
                    value: CrossoverModel.ripple
                    unit: "dB"
                    valueColor: Material.accent
                    buttonsVisible: false
                    Layout.bottomMargin: 6
                }
            ]

            property var lowPassSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "Low pass", chart.xAxisLog, chart.yAxis)
            property var highPassSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "High pass", chart.xAxisLog, chart.yAxis)
            property var sumSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "Sum", chart.xAxisLog, chart.yAxis)

            Component.onCompleted: {
                chart.xAxis.min = 0.0
                chart.xAxis.max = 240.0

                chart.lowPassSeries.color = chart.foregroundColor
                chart.lowPassSeries.width = 1.0
                chart.highPassSeries.color = chart.foregroundColor
                chart.highPassSeries.width = 1.0
                chart.sumSeries.color = chart.accentColor
                chart.sumSeries.width = 1.0

                chart.handles.color = "transparent"
                chart.handles.borderWidth = 1.0
                chart.handles.borderColor = chart.foregroundColor
                chart.handles.markerSize = 9.0
                // F and Q handles
                chart.handles.append(160, -3.0)
                chart.handles.append(176.0, -3.0)
                // Gain handles
                chart.handles.append(6, 0.0)
                chart.handles.append(234.0, 0.0)
                CrossoverModel.setHandles(chart.handles)
                CrossoverModel.setLowPassSeries(lowPassSeries)
                CrossoverModel.setHighPassSeries(highPassSeries)
                CrossoverModel.setSumSeries(sumSeries)
            }
        }
    }
}
