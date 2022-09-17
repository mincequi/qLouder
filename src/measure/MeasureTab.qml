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

            // Time per octave
            Label {
                leftPadding: 9
                height: 24
                text: "Time per octave"
                font.pixelSize: 12
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
                font.pixelSize: 12
            }
            SmallSpinBox {
                value: -6
                from: -24
                to: 0
                onValueChanged: MeasureModel.setLevel(value)
            }

            // Channels
            ToolSeparator {
                implicitHeight: 24
                horizontalPadding: 9
            }
            Label {
                height: 24
                text: "Channels"
                font.pixelSize: 12
            }
            SmallComboBox {
                id: channelsComboBox
                flat: true
                model: MeasureModel.channels
                onCurrentIndexChanged: MeasureModel.setChannels(currentIndex)
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
                    font.pixelSize: 12
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
