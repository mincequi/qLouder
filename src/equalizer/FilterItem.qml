import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import ".."
import "../ui"

import EqualizerModel 1.0

ColumnLayout {
    id: item
    width: 96
    property var chart2
    property var model

    spacing: 0
    RowLayout {
        spacing: 0
        Label {
            id: label
            text: "Filter " + (index + 1).toString()
            height: 24
            leftPadding: 6
            font.bold: true
            font.pointSize: 12
        }
        Item {
            Layout.fillWidth: true
        }
        SmallToolButton {
            id: deleteFilter
            iconName: "close"
            iconColor: Material.color(Material.Red, Material.Shade300)
            flat: true
            onClicked: {
                chart2.removeSeries(model.response)
                EqualizerModel.removeFilter(index)
            }
        }
    }
    UpDownSpinBox {
        label: "F"
        value: model.f
        onValueUp: EqualizerModel.stepF(index, +1)
        onValueDown: EqualizerModel.stepF(index, -1)
    }
    UpDownSpinBox {
        label: "Q"
        value: model.q.toPrecision(3)
        onValueUp: EqualizerModel.stepQ(index, -0.5)
        onValueDown: EqualizerModel.stepQ(index, +0.5)
    }
    UpDownSpinBox {
        label: "G"
        value: model.g.toFixed(1) + " dB"
        onValueUp: EqualizerModel.stepG(index, +0.2)
        onValueDown: EqualizerModel.stepG(index, -0.2)
    }
    ToolSeparator {
        anchors.left: parent.left
        anchors.right: parent.right
        topPadding: 0
        bottomPadding: 0
        orientation: Qt.Horizontal
    }
}
