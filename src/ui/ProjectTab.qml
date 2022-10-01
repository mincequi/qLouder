import QtQuick 2.4
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import PlayerModel 1.0
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
                font.pixelSize: 12
            }
            Label {
                id: projectDir
                Layout.fillWidth: true
                leftPadding: 6
                rightPadding: 6
                font.pixelSize: 12
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
                implicitWidth: 84
                iconName: "folder"
                text: "Open..."
            }

            Label {
                text: "Microphone calibration file 0°"
                font.pixelSize: 12
            }
            Label {
                id: micFile0
                Layout.fillWidth: true
                leftPadding: 6
                rightPadding: 6
                font.pixelSize: 12
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
                implicitWidth: 84
                iconName: "file"
                text: "Open..."
                onClicked: {
                    fileDialog.degrees = 0
                    fileDialog.open()
                }
            }

            Label {
                text: "Microphone calibration file 90°"
                font.pixelSize: 12
            }
            Label {
                id: micFile90
                Layout.fillWidth: true
                leftPadding: 6
                rightPadding: 6
                font.pixelSize: 12
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
                implicitWidth: 84
                iconName: "file"
                text: "Open..."
                onClicked: {
                    fileDialog.degrees = 90
                    fileDialog.open()
                }
            }

            Label {
                text: "Sample audio file"
                font.pixelSize: 12
            }
            Label {
                id: sampleFile
                Layout.fillWidth: true
                leftPadding: 6
                rightPadding: 6
                font.pixelSize: 12
                text: PlayerModel.file
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
                implicitWidth: 84
                iconName: "file-music"
                text: "Open..."
                onClicked: {
                    sampleFileDialog.open()
                }
            }
        }

        GridLayout {
            columns: 4
            columnSpacing: 12
            Layout.topMargin: 24

            Label {
                text: "Audio input device"
                font.pixelSize: 12
            }
            SmallComboBox {
                id: inputDevices
                model: ProjectModel.inputDevices
                sizeModel: ProjectModel.outputDevices   // Yes, the outputDevices to have a common width with input devices
                onCurrentIndexChanged: ProjectModel.setInputDevice(currentIndex)
                Component.onCompleted: currentIndex = ProjectModel.savedInputDevice
            }
            Label {
                Layout.leftMargin: 24
                text: "Sample rate"
                font.pixelSize: 12
            }
            SmallComboBox {
                model: ProjectModel.inputSampleRates
                sizeModel: ProjectModel.inputSampleRates
                onCurrentIndexChanged: ProjectModel.setInputSampleRate(currentIndex)
                Component.onCompleted: currentIndex = ProjectModel.savedInputSampleRate
            }

            Label {
                text: "Audio output device"
                font.pixelSize: 12
            }
            SmallComboBox {
                model: ProjectModel.outputDevices
                sizeModel: ProjectModel.outputDevices
                onCurrentIndexChanged: ProjectModel.setOutputDevice(currentIndex)
                Component.onCompleted: currentIndex = ProjectModel.savedOutputDevice
            }
            Label {
                Layout.leftMargin: 24
                text: "Sample rate"
                font.pixelSize: 12
            }
            SmallComboBox {
                model: ProjectModel.outputSampleRates
                sizeModel: ProjectModel.inputSampleRates
                onCurrentIndexChanged: ProjectModel.setOutputSampleRate(currentIndex)
                Component.onCompleted: currentIndex = ProjectModel.savedOutputSampleRate
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

    FileDialog {
        id: sampleFileDialog
        title: "Music file"
        folder: shortcuts.music
        nameFilters: PlayerModel.supportedFileExtensions
        onAccepted: {
            var path = sampleFileDialog.fileUrl.toString();
            // remove prefixed "file:///"
            path = path.replace(/^(file:\/{3})/,"/");
            // unescape html codes like '%23' for '#'
            var cleanPath = decodeURIComponent(path);
            PlayerModel.setFile(cleanPath)
        }
    }
}
