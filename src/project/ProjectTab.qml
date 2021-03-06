import QtQuick 2.4
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.15

import ".."

import ProjectModel 1.0

Page {
    header: Row {
        height: 24
        width: parent.width
        ToolBar {
            height: parent.height
            width: parent.width
            Material.elevation: 0
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 24
        anchors.leftMargin: 48
        anchors.rightMargin: 48
        GridLayout {
            columns: 3
            columnSpacing: 12

            Label {
                text: "Project directory"
                font.pointSize: 12
            }
            Label {
                id: projectDir
                Layout.fillWidth: true
                leftPadding: 6
                rightPadding: 6
                font.pointSize: 12
                text: ProjectModel.projectDir
                background: Rectangle {
                    y: -4.5
                    height: 24
                    color: "transparent"
                    border.color: Material.frameColor
                    border.width: 1
                }
            }
            SmallToolButton {
                iconName: "folder"
                text: "Open..."
            }

            Label {
                text: "Microphone calibration file 0°"
                font.pointSize: 12
            }
            Label {
                id: micFile0
                Layout.fillWidth: true
                leftPadding: 6
                rightPadding: 6
                font.pointSize: 12
                text: ProjectModel.micCalibration0
                elide: Text.ElideLeft
                background: Rectangle {
                    y: -4.5
                    height: 24
                    color: "transparent"
                    border.color: Material.frameColor
                    border.width: 1
                }
            }
            SmallToolButton {
                iconName: "file"
                text: "Open..."
                onClicked: {
                    fileDialog.degrees = 0
                    fileDialog.open()
                }
            }

            Label {
                text: "Microphone calibration file 90°"
                font.pointSize: 12
            }
            Label {
                id: micFile90
                Layout.fillWidth: true
                leftPadding: 6
                rightPadding: 6
                font.pointSize: 12
                text: ProjectModel.micCalibration90
                elide: Text.ElideLeft
                background: Rectangle {
                    y: -4.5
                    height: 24
                    color: "transparent"
                    border.color: Material.frameColor
                    border.width: 1
                }
            }
            SmallToolButton {
                iconName: "file"
                text: "Open..."
                onClicked: {
                    fileDialog.degrees = 90
                    fileDialog.open()
                }
            }
        }

        GridLayout {
            columns: 4
            columnSpacing: 12
            Layout.topMargin: 24

            Label {
                text: "Audio input device"
                font.pointSize: 12
            }
            SmallComboBox {
                id: inputDevices
                model: ProjectModel.inputDevices
                sizeModel: ProjectModel.outputDevices
                onCurrentIndexChanged: ProjectModel.setInputDevice(currentIndex)
            }
            Label {
                Layout.leftMargin: 24
                text: "Sample rate"
                font.pointSize: 12
            }
            SmallComboBox {
                model: ProjectModel.inputSampleRates
                sizeModel: ProjectModel.outputSampleRates
                onCurrentIndexChanged: ProjectModel.setInputSampleRate(currentIndex)
            }

            Label {
                text: "Audio output device"
                font.pointSize: 12
            }
            SmallComboBox {
                model: ProjectModel.outputDevices
                sizeModel: ProjectModel.inputDevices
                onCurrentIndexChanged: ProjectModel.setOutputDevice(currentIndex)
            }
            Label {
                Layout.leftMargin: 24
                text: "Sample rate"
                font.pointSize: 12
            }
            SmallComboBox {
                model: ProjectModel.outputSampleRates
                sizeModel: ProjectModel.inputSampleRates
                onCurrentIndexChanged: ProjectModel.setOutputSampleRate(currentIndex)
            }

            Item {
                Layout.columnSpan: 4
                Layout.fillHeight: true
            }
        }
    }

    FileDialog {
        id: fileDialog
        property int degrees: 0
        title: "Calibration file"
        folder: shortcuts.home
        selectExisting: true
        selectMultiple: false
        onAccepted: {
            var path = fileDialog.fileUrl.toString();
            // remove prefixed "file:///"
            path = path.replace(/^(file:\/{3})/,"/");
            // unescape html codes like '%23' for '#'
            var cleanPath = decodeURIComponent(path);
            ProjectModel.openCalibrationFile(degrees, cleanPath)
        }
    }
}
