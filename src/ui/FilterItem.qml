import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

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
            font.pixelSize: 12
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
    RowLayout {
        id: control
        spacing: 0
        Layout.leftMargin: 0
        Layout.rightMargin: 0

        Label {
            width: 24
            height: 24
            text: "T"
            leftPadding: 6
            font.pixelSize: 10
            Layout.alignment: Qt.AlignBaseline
        }

        SmallComboBox {
            font.pixelSize: 10
            flat: true
            model: EqualizerModel.types
            currentIndex: item.model.type
            onCurrentIndexChanged: EqualizerModel.setType(index, currentIndex)
            Layout.alignment: Qt.AlignRight
            Layout.fillWidth: true
            Layout.leftMargin: 3
        }
    }

    UpDownSpinBox {
        label: "F"
        value: model.f
        unit: model.fUnit
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
        enabled: model.isGainEnabled
        value: model.g.toFixed(1)
        unit: "dB"
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
