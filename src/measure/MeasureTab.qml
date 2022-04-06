import QtQuick 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15

import ".."
import "../charts"

import FrChartModel 1.0
import MeasureModel 1.0

Page {
    function colorWithAlpha(color, alpha) {
        return Qt.rgba(color.r, color.g, color.b, alpha)
    }

    //Layout.fillWidth: parent
    //Layout.fillHeight: parent

    //anchors.fill: parent

    header: ToolBar {
        height: 24
        width: parent.width
        Material.elevation: 0
        leftPadding: 0
        spacing: 3

        // Progress bar
        Rectangle {
            y: 23
            width: parent.width - 96
            height: (MeasureModel.progress > 0.0 && MeasureModel.progress < 1.0) ? 1 : 0
            color: colorWithAlpha(Material.accentColor, 0.33)
            Rectangle {
                y: -1
                width: parent.width * MeasureModel.progress
                height: 2
                color: Material.accentColor
            }
        }

        // Tool bar
        RowLayout {
            anchors.left: parent.left
            width: parent.width
            layoutDirection: "LeftToRight"
            spacing: 0

            // Time per octave
            Label {
                leftPadding: 9
                height: 24
                text: "Time per octave"
                font.pointSize: 12
            }
            SmallComboBox {
                flat: true
                model: MeasureModel.lengths
                onCurrentIndexChanged: MeasureModel.setLength(currentIndex)
            }

            // Level
            ToolSeparator {
                leftPadding: 0
                implicitHeight: 24
                horizontalPadding: 9
            }
            Label {
                height: 24
                text: "Level"
                font.pointSize: 12
            }
            SmallSpinBox {
                value: -6
                from: -24
                to: 0
                onValueChanged: MeasureModel.setLevel(value)
            }

            // Frequency range
            ToolSeparator {
                leftPadding: 0
                implicitHeight: 24
                horizontalPadding: 9
            }
            Label {
                height: 24
                text: "Range"
                font.pointSize: 12
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
                    text: MeasureModel.minFrequencyReadout
                    font.pointSize: 12
                    font.bold: true
                }
            }
            SmallRangeSlider {
                implicitWidth: 96
                from: 0
                to: MeasureModel.frequencies.length-1
                snapMode: RangeSlider.SnapAlways
                stepSize: 1
                first.value: MeasureModel.minFrequencySlider
                second.value: MeasureModel.maxFrequencySlider
                first.onVisualPositionChanged: MeasureModel.setMinFrequencySlider(first.value)
                second.onVisualPositionChanged: MeasureModel.setMaxFrequencySlider(second.value)
            }
            Item {
                id: maxFrequencyReadout
                height: 24
                width: textMetrics.width + 6

                Label {
                    topPadding: 5
                    height: 24
                    anchors.right: parent.right
                    text: MeasureModel.maxFrequencyReadout
                    font.pointSize: 12
                    font.bold: true
                }
            }

            // Spacer item
            Item {
                Layout.fillWidth: true
            }

            /*
            SmallToolButton {
                id: saveButton
                text: "Save"
                iconName: "content-save"
                iconColor: Material.foreground
                onClicked: fileDialog.open()
            }
            */

            Rectangle {
                height: 24
                width: 1
                color: Material.background
            }

            SmallToolButton {
                id: measureButton
                TextMetrics {
                    id: measureButtonMetrics
                    font: measureButton.font
                    text: "Measure"
                }

                implicitWidth: 96 //measureButtonMetrics.width + 36
                text: MeasureModel.measureButtonText
                iconName: MeasureModel.measureButtonIcon
                iconColor: Material.color(Material.Red, Material.Shade300)
                onClicked: MeasureModel.onMeasureButtonClicked()
            }
        }
    }

    SplitView {
        anchors.topMargin: 1
        anchors.fill: parent
        orientation: Qt.Vertical

        IrChart {
            SplitView.minimumHeight: 144
            SplitView.preferredHeight: 160
            SplitView.maximumHeight: 192
        }
        FrChart {
            id: frChart
            model: FrChartModel {
            }
            toolBarChildren: [
                RowLayout {
                    spacing: 0
                    Layout.topMargin: 6

                    SmallToolButton {
                        iconName: "magnify-minus"
                        flat: true
                        onClicked: frChart.model.zoom(0.5);
                    }
                    SmallToolButton {
                        id: zoomIn
                        iconName: "magnify-plus"
                        flat: true
                        onClicked: frChart.model.zoom(2.0);
                    }
                    SmallToolButton {
                        iconName: "chevron-left"
                        flat: true
                        onClicked: frChart.model.pan(-0.2);
                    }
                    SmallToolButton {
                        iconName: "chevron-right"
                        flat: true
                        onClicked: frChart.model.pan(0.2);
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
                    model: frChart.model.calibrations
                    //model: [ "None", "0°", "90°" ]
                    onCurrentIndexChanged: frChart.model.setCalibration(currentIndex)
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
        }
    }

    /*
    Label {
        text: MeasureModel.level
        color: Material.accent
        anchors.right: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: Text.AlignRight
        font.pointSize: 48
        visible: MeasureModel.progress > 0.0
    }
    */

    Rectangle {
        color: colorWithAlpha(Material.background, 0.66)
        opacity: MeasureModel.progress > 0.0 ? 1.0 : 0.0
        anchors.fill: parent

        Gauge {
            id: gauge
            radius: 72
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: -48
            value: MeasureModel.level
            valueMax: MeasureModel.levelMax
            valueWarning: 0.0
            valueRangeMin: -96.0
            valueRangeMax: 12.0
            opacity: MeasureModel.progress > 0.0 ? 1.0 : 0.0

            Behavior on opacity { SmoothedAnimation { duration: 250; velocity: -1; easing.type: "OutQuart" } }
        }
    }

    Dialog {
        Connections {
            target: MeasureModel
            onErrorOccurred: {
                console.log("error occurred")
                dialog.open()
            }
        }

        id: dialog
        modal: true
        Overlay.modal: Rectangle {
            color: colorWithAlpha(Material.background, 0.66)
        }
        title: MeasureModel.errorString
        standardButtons: Dialog.Retry | Dialog.Cancel
        anchors.centerIn: parent
        width: 480

        Text {
            anchors.fill: parent
            color: Material.foreground
            text: MeasureModel.errorDescription
            wrapMode: Text.WordWrap
        }

        onAccepted: MeasureModel.onMeasureButtonClicked()
        //onRejected: console.log("Cancel clicked")
    }

    FileDialog {
        id: fileDialog
        title: "Export signal"
        folder: shortcuts.home
        selectExisting: false
        selectMultiple: false
        onAccepted: {
            MeasureModel.saveFile(fileDialog.fileUrl)
        }
    }
}
