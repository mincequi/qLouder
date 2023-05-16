import QtQuick 2.12
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtCharts 2.3

import TargetModel 1.0

import "items"

Page {
    header: ToolBar {
        height: 24
        width: parent.width
        Material.elevation: 0
    }

    ColumnLayout {
        anchors.topMargin: 1
        anchors.fill: parent

        FrChart {
            id: chart
            model: TargetModel
            toolBarChildren: [
                Label {
                    height: 24
                    padding: 6
                    text: "Loudness"
                    font.bold: true
                    font.pixelSize: 12
                },
                MicroUpDownSpinBox {
                    //label: "F"
                    value: TargetModel.loudness.toFixed(1) + " phon"
                    onValueUp: TargetModel.stepParam(0, 0, +0.5)
                    onValueDown: TargetModel.stepParam(0, 0, -0.5)
                },

                //Label {
                //    height: 24
                //    padding: 6
                //    text: "Harman"
                //    font.bold: true
                //    font.pixelSize: 12
                //},
                //MicroUpDownSpinBox {
                //    value: TargetModel.harman
                //    onValueUp: TargetModel.stepParam(1, 0, +0.25)
                //    onValueDown: TargetModel.stepParam(1, 0, -0.25)
                //},
//
                //ToolSeparator {
                //    anchors.left: parent.left
                //    anchors.right: parent.right
                //    topPadding: 6
                //    bottomPadding: 6
                //    orientation: Qt.Horizontal
                //},

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
                    text: "Sum"
                    font.bold: true
                    font.pixelSize: 12
                },

                MicroUpDownSpinBox {
                    label: "Min"
                    value: TargetModel.sumMin.toFixed(1)
                    unit: "dB"
                    valueColor: Material.accent
                    buttonsVisible: false
                },
                MicroUpDownSpinBox {
                    label: "Max"
                    value: TargetModel.sumMax.toFixed(1)
                    unit: "dB"
                    valueColor: Material.accent
                    buttonsVisible: false
                    Layout.bottomMargin: 6
                }
            ]

            property var loudnessSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "Loudness", chart.xAxisLog, chart.yAxis)
            property var harmanSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "Harman", chart.xAxisLog, chart.yAxis)
            property var sumSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "Sum", chart.xAxisLog, chart.yAxis)

            Component.onCompleted: {
                chart.xAxis.min = 0.0
                chart.xAxis.max = 240.0

                chart.loudnessSeries.color = chart.foregroundColor
                chart.loudnessSeries.width = 1.0
                chart.harmanSeries.color = chart.foregroundColor
                chart.harmanSeries.width = 1.0
                chart.sumSeries.color = chart.accentColor
                chart.sumSeries.width = 1.0

                chart.handles.color = "transparent"
                chart.handles.borderWidth = 1.0
                chart.handles.borderColor = chart.foregroundColor
                chart.handles.markerSize = 9.0
                // Gain handles
                chart.handles.append(24, 0.0)
                //chart.irWindowHandles.append(234.0, 0.0)
                TargetModel.setHandles(chart.handles)
                TargetModel.setLoudnessSeries(loudnessSeries)
                //TargetModel.setHarmanSeries(harmanSeries)
                TargetModel.setSumSeries(sumSeries)
            }
        }
    }
}
