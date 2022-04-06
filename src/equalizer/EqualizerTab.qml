import QtQuick 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15

import ".."
import "../charts"
import "../ui"

import EqualizerModel 1.0

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
            model: EqualizerModel
            property var sumSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "Sum", chart.xAxisLog, chart.yAxis)

            toolBarChildren: [
                ListView {
                    id: listView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    model: EqualizerModel.filters
                    delegate: FilterItem {
                        chart2: chart.chart
                        model: modelData
                    }
                },
                Rectangle {
                    width: 96
                    height: 1
                    color: Material.background
                },
                SmallToolButton {
                    height: 24
                    implicitWidth: 96
                    text: "Add filter"
                    iconName: "plus"
                    onClicked: {
                        var response = chart.chart.createSeries(ChartView.SeriesTypeLine, "Response", chart.xAxisLog, chart.yAxis)
                        response.color = chart.foregroundColor
                        response.width = 1.0

                        // TODO: we have to delete sumSeries each filter creation to keep z ordering
                        chart.chart.removeSeries(chart.sumSeries)
                        chart.sumSeries = chart.chart.createSeries(ChartView.SeriesTypeLine, "Sum", chart.xAxisLog, chart.yAxis)
                        chart.sumSeries.color = chart.accentColor
                        chart.sumSeries.width = 1.0

                        EqualizerModel.addFilter(response)
                        EqualizerModel.setSumSeries(chart.sumSeries)
                    }
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
                    font.pointSize: 12
                },
                UpDownSpinBox {
                    label: "Min"
                    value: EqualizerModel.sumMin.toFixed(1) + " dB"
                    valueColor: Material.accent
                    buttonsVisible: false
                },
                UpDownSpinBox {
                    label: "Max"
                    value: EqualizerModel.sumMax.toFixed(1) + " dB"
                    valueColor: Material.accent
                    buttonsVisible: false
                    Layout.bottomMargin: 6
                }
            ]

            Component.onCompleted: {
                chart.handles.color = "transparent"
                chart.handles.borderWidth = 1.0
                chart.handles.borderColor = chart.foregroundColor
                chart.handles.markerSize = 9.0
                chart.handles.append(model.irWindowLeft, 0.0)
                chart.handles.append(model.irWindowRight, 0.0)
                EqualizerModel.setHandles(chart.handles)

                chart.xAxis.min = 0.0
                chart.xAxis.max = 240.0

                chart.sumSeries.color = chart.accentColor
                chart.sumSeries.width = 1.0

                EqualizerModel.setSumSeries(sumSeries)
            }
        }
    }
}
