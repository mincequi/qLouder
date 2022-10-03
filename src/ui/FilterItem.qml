import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import EqualizerModel 1.0

import "items"

ColumnLayout {
    id: item
    width: 96
    property var chart2
    property var filterModel
    property var itemHeight: 18

    spacing: 0
    RowLayout {
        spacing: 0
        Label {
            id: label
            text: "Filter " + (index + 1).toString()
            height: itemHeight
            leftPadding: 6
            font.pixelSize: 12
        }
        Item {
            Layout.fillWidth: true
        }
        SmallToolButton {
            id: deleteFilter
            implicitHeight: itemHeight
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
        spacing: 0
        Layout.leftMargin: 0
        Layout.rightMargin: 0

        Label {
            width: 24
            height: itemHeight
            text: "T"
            leftPadding: 6
            font.pixelSize: 10
            Layout.alignment: Qt.AlignBaseline
        }
        MicroComboBox {
            id: typeComboBox
            implicitHeight: itemHeight
            font.pixelSize: 10
            model: ["None", "Peaking", "Low Pass", "High Pass", "Low Shelf", "High Shelf"]
            onCurrentIndexChanged: {
                //console.log("onCurrentIndexChanged>", currentIndex)
                filterModel.setType(currentIndex)
            }
            currentIndex: filterModel.type
            Layout.fillWidth: true
            Layout.leftMargin: 3
        }
    }

    UpDownSpinBox {
        label: "F"
        enabled: model.isFrequencyAvailable
        value: model.f
        unit: model.fUnit
        onValueUp: EqualizerModel.stepF(index, +1)
        onValueDown: EqualizerModel.stepF(index, -1)
    }
    UpDownSpinBox {
        label: "Q"
        enabled: model.isQAvailable
        value: model.q.toPrecision(3)
        onValueUp: EqualizerModel.stepQ(index, -0.5)
        onValueDown: EqualizerModel.stepQ(index, +0.5)
    }
    UpDownSpinBox {
        label: "G"
        enabled: model.isGainAvailable
        value: model.g.toFixed(1)
        unit: "dB"
        onValueUp: EqualizerModel.stepG(index, +0.2)
        onValueDown: EqualizerModel.stepG(index, -0.2)
    }
    ToolSeparator {
        Layout.fillWidth: true
        topPadding: 0
        bottomPadding: 0
        orientation: Qt.Horizontal
    }
}
