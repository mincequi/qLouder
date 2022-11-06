import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import Qt.labs.settings 1.0

import MeasureModel 1.0
import PlayerBarModel 1.0

ApplicationWindow {
    function colorWithAlpha(color, alpha) {
        return Qt.rgba(color.r, color.g, color.b, alpha)
    }

    id: window
    minimumWidth: 768
    minimumHeight: 480 // 432 would be nice
    visible: true
    title: "qLouder"

    Settings {
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }

    Material.theme: Material.Dark
    Material.primary: Material.color(Material.Grey, Material.Shade800)
    Material.accent: Material.DeepOrange

    ToolBar {
        id: upperLeftToolBar
        width: 72
        height: 24
        Material.elevation: 0
    }

    VerticalTabBar {
        id: bar
        currentIndex: 1
        spacing: 0
        anchors.left: parent.left
        anchors.top: upperLeftToolBar.bottom
        anchors.topMargin: 1
        anchors.bottom: playerControls.top
        anchors.bottomMargin: 1
        width: 72

        MaterialTabButton {
            text: "Settings"
            icon.name: "wrench"
        }
        MaterialTabButton {
            text: "Measure"
            icon.name: "microphone-variant"
        }
        MaterialTabButton {
            text: "Equalizer"
            icon.name: "tune-vertical"
        }
        MaterialTabButton {
            text: "Crossover"
            icon.name: "swap-horizontal"
        }
        MaterialTabButton {
            text: "Export"
            icon.name: "export"
        }
        //MaterialTabButton {
        //    text: "Target"
        //    icon.name: "bullseye"
        //}
    }

    StackLayout {
        //anchors.fill: parent
        anchors.left: bar.right
        anchors.leftMargin: 1
        //anchors.rightMargin: 73
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: playerControls.top
        anchors.bottomMargin: 1
        currentIndex: bar.currentIndex
        ProjectTab {
        }
        MeasureTab {
        }
        EqualizerTab {
        }
        CrossoverTab {
        }
        ExportTab {
        }

        TargetTab {
        }
    }

    Rectangle {
        color: colorWithAlpha(Material.background, 0.66)
        opacity: MeasureModel.progress > 0.0 ? 1.0 : 0.0
        anchors.left: bar.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: playerBar.top

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

    ToolBar {
        id: playerControls
        width: 72
        height: 48
        anchors.bottom: lowerLeftToolBar.top
        anchors.bottomMargin: 1
        Material.elevation: 0

        ColumnLayout {
            anchors.fill: parent
            spacing: 0
            SmallToolButton {
                id: measureButton
                font.pixelSize: 9
                implicitWidth: 72
                enabled: !PlayerBarModel.isPlaying
                text: MeasureModel.measureButtonText
                iconName: MeasureModel.measureButtonIcon
                iconColor: Material.color(Material.Red, Material.Shade300)
                onClicked: PlayerBarModel.toggleMeasure()
            }
            SmallToolButton {
                id: playButton
                font.pixelSize: 9
                implicitWidth: 72
                enabled: !PlayerBarModel.isMeasuring
                text: PlayerBarModel.isPlaying ? "Pause" : "Play"
                iconName: PlayerBarModel.isPlaying ? "pause" : "play"
                onClicked: PlayerBarModel.togglePlayPause()
            }
        }
    }

    PlayerBar {
        id: playerBar
        anchors.left: bar.right
        anchors.leftMargin: 1
        anchors.right: parent.right
        anchors.rightMargin: 97
        anchors.bottom: statusBar.top
        anchors.bottomMargin: 1
    }

    ToolBar {
        id: lowerLeftToolBar
        width: 72
        height: 24
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        Material.elevation: 0
    }
    ToolBar {
        id: lowerRightToolBar
        width: 96
        height: 48
        anchors.right: parent.right
        anchors.bottom: lowerRightSpacerToolBar.top
        anchors.bottomMargin: 1
        Material.elevation: 0
    }
    ToolBar {
        id: lowerRightSpacerToolBar
        width: 96
        height: 24
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        Material.elevation: 0
    }

    StatusBar {
        id: statusBar
        height: 24
        anchors.left: lowerLeftToolBar.right
        anchors.leftMargin: 1
        anchors.right: parent.right
        anchors.rightMargin: 97
        anchors.bottom: parent.bottom
    }
}
