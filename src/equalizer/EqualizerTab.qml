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
import TargetModel 1.0

Page {
    function colorWithAlpha(color, alpha) {
        return Qt.rgba(color.r, color.g, color.b, alpha)
    }

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

            Label {
                height: 24
                leftPadding: 9
                text: "Level"
                font.pixelSize: 12
            }
            SmallSpinBox {
                value: -12
                from: -24
                to: 0
                onValueModified: EqualizerModel.setLevel(value)
                Component.onCompleted: EqualizerModel.setLevel(value)
            }

            // Frequency range
            // Level
            ToolSeparator {
                leftPadding: 0
                implicitHeight: 24
                horizontalPadding: 9
            }
            Label {
                height: 24
                text: "Range"
                font.pixelSize: 12
            }
            TextMetrics {
                id: textMetrics
                font: minFrequencyReadout.font
                text: "250 Hz"
            }
            Item {
                height: 24
                width: textMetrics.width + 6

                Label {
                    id: minFrequencyReadout
                    topPadding: 5
                    height: 24
                    anchors.right: parent.right
                    text: EqualizerModel.minFrequencyReadout
                    font.pixelSize: 12
                    font.bold: true
                }
            }
            SmallRangeSlider {
                implicitWidth: 96
                from: 0
                to: EqualizerModel.frequencies.length-1
                snapMode: RangeSlider.SnapAlways
                stepSize: 1
                first.value: EqualizerModel.minFrequencySlider
                second.value: EqualizerModel.maxFrequencySlider
                first.onVisualPositionChanged: EqualizerModel.setMinFrequencySlider(first.value)
                second.onVisualPositionChanged: EqualizerModel.setMaxFrequencySlider(second.value)
            }
            Item {
                id: maxFrequencyReadout
                height: 24
                width: textMetrics.width + 6

                Label {
                    topPadding: 5
                    height: 24
                    anchors.right: parent.right
                    text: EqualizerModel.maxFrequencyReadout
                    font.pixelSize: 12
                    font.bold: true
                }
            }

            // Loudness
            ToolSeparator {
                implicitHeight: 24
                horizontalPadding: 9
            }
            Label {
                height: 24
                text: "Loudness"
                font.pixelSize: 12
            }
            SmallSpinBox {
                value: 0
                from: 0
                to: 40
                unit: "phon"
                implicitWidth: 96
                onValueModified: TargetModel.setLoudness(value)
                Component.onCompleted: TargetModel.setLoudness(value)
            }

            // Spacer item
            Item {
                Layout.fillWidth: true
            }
            SmallToolButton {
                height: 24
                implicitWidth: 96
                text: "Optimize"
                //iconName: "arrow-all"
                iconName: "auto-fix"
                onClicked: {
                    EqualizerModel.optimize()
                }
            }
            Rectangle {
                height: 24
                width: 1
                color: Material.background
            }
            SmallToolButton {
                height: 24
                implicitWidth: 96
                text: "Add filter"
                iconName: "plus"
                onClicked: {
                    var response = chart.chart.createSeries(ChartView.SeriesTypeLine, "Response", chart.xAxisLog, chart.yAxis)
                    response.color = Material.color(Material.Grey)
                    response.width = 1.0
                    EqualizerModel.addFilter(response)

                    // TODO: we have to delete sumSeries each filter creation to keep z ordering
                    chart.chart.removeSeries(chart.sumSeries)
                    chart.sumSeries = chart.chart.createSeries(ChartView.SeriesTypeLine, "Sum", chart.xAxisLog, chart.yAxis)
                    chart.sumSeries.color = chart.accentColor
                    chart.sumSeries.width = 1.0
                    EqualizerModel.setFilterSumSeries(chart.sumSeries)
                }
            }
        }
    }

    ColumnLayout {
        anchors.topMargin: 1
        anchors.fill: parent

        FrChart {
            id: chart
            model: EqualizerModel
            property var areaSeries: chart.chart.createSeries(ChartView.SeriesTypeArea, "Area", chart.xAxisLog, chart.yAxis)
            property var sumSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "Sum", chart.xAxisLog, chart.yAxis)
            property var targetSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "Target", chart.xAxisLog, chart.yAxis)
            property var measurementSeries: chart.chart.createSeries(ChartView.SeriesTypeLine, "Measurement", chart.xAxisLog, chart.yAxis)

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
                Label {
                    height: 24
                    leftPadding: 6
                    topPadding: 6
                    bottomPadding: 6
                    text: "Sum"
                    font.bold: true
                    font.pixelSize: 12
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
                chart.areaSeries.upperSeries = chart.targetSeries
                chart.areaSeries.lowerSeries = chart.measurementSeries
                chart.areaSeries.borderColor = "transparent"
                chart.areaSeries.borderWidth = 4.0
                chart.areaSeries.color = colorWithAlpha(chart.accentColor, 0.33)

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
                EqualizerModel.setFilterSumSeries(sumSeries)

                chart.targetSeries.color = Material.color(Material.Amber, Material.Shade200)
                chart.targetSeries.width = 1.0
                EqualizerModel.setTargetSeries(targetSeries)

                chart.measurementSeries.color = Material.color(Material.Orange, Material.Shade200)
                chart.measurementSeries.width = 1.0
                EqualizerModel.setFilteredMeasurementSeries(measurementSeries)
            }
        }
    }
}
