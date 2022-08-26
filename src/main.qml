import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.15
import QtQuick.Window 2.12
import Qt.labs.settings 1.0

import "crossover"
import "equalizer"
import "project"
import "measure"
import "status"
import "target"

ApplicationWindow {
    id: window
    width: 768
    height: 480
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
        width: 72
        height: 24
        Material.elevation: 0
    }

    VerticalTabBar {
        id: bar
        currentIndex: 1
        spacing: 0
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 25
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 25
        width: 72

        MaterialTabButton {
            text: "Settings"
            icon.name: "wrench"
        }
        MaterialTabButton {
            text: "Measure"
            icon.name: "microphone-variant"
        }
        //MaterialTabButton {
        //    text: "Target"
        //    icon.name: "bullseye"
        //}
        MaterialTabButton {
            text: "Equalizer"
            icon.name: "tune-vertical"
        }
        MaterialTabButton {
            text: "Crossover"
            icon.name: "swap-horizontal"
        }
    }

    StackLayout {
        //anchors.fill: parent
        anchors.left: bar.right
        anchors.leftMargin: 1
        //anchors.rightMargin: 73
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 25
        currentIndex: bar.currentIndex
        ProjectTab {
        }
        MeasureTab {
        }
        EqualizerTab {
        }
        CrossoverTab {
        }

        TargetTab {
        }
    }

    ToolBar {
        width: 72
        height: 24
        anchors.bottom: parent.bottom
        Material.elevation: 0
    }

    StatusBar {
        anchors.left: bar.right
        anchors.leftMargin: 1
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
