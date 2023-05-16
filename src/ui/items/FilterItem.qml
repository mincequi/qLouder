import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import EqualizerModel 1.0

Rectangle {
    property var chart2
    property var filterModel
    id: control
    implicitWidth: 96
    implicitHeight: 84
    color: Material.primary
    //radius: 6.0

ColumnLayout {
    id: item
    width: 96
    property int itemHeight: 18

    spacing: 0
    RowLayout {
        spacing: 0
        Label {
            id: label
            text: "Filter " + (index + 1).toString()
            height: item.itemHeight
            leftPadding: 6
            font.pixelSize: 10
        }
        Item {
            Layout.fillWidth: true
        }
        MicroToolButton {
            id: deleteFilter
            implicitHeight: item.itemHeight
            iconName: "close"
            iconColor: Material.color(Material.Red, Material.Shade300)
            flat: true
            onClicked: {
                control.chart2.removeSeries(model.response)
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
            height: item.itemHeight
            text: "T"
            leftPadding: 6
            font.pixelSize: 10
            Layout.alignment: Qt.AlignBaseline
        }
        MicroComboBox {
            id: typeComboBox
            implicitHeight: item.itemHeight
            font.pixelSize: 10
            model: ["None", "Peaking", "Low Pass", "High Pass", "Low Shelf", "High Shelf"]
            onCurrentIndexChanged: {
                //console.log("onCurrentIndexChanged>", currentIndex)
                control.filterModel.setType(currentIndex)
            }
            currentIndex: control.filterModel.type
            Layout.fillWidth: true
            Layout.leftMargin: 3
        }
    }

    MicroUpDownSpinBox {
        label: "F"
        enabled: model.isFrequencyAvailable
        value: model.f
        unit: model.fUnit
        onValueUp: EqualizerModel.stepF(index, +1)
        onValueDown: EqualizerModel.stepF(index, -1)
    }
    MicroUpDownSpinBox {
        label: "Q"
        enabled: model.isQAvailable
        value: model.q.toPrecision(3)
        onValueUp: EqualizerModel.stepQ(index, -0.5)
        onValueDown: EqualizerModel.stepQ(index, +0.5)
    }
    MicroUpDownSpinBox {
        label: "G"
        enabled: model.isGainAvailable
        value: model.g.toFixed(1)
        unit: "dB"
        onValueUp: EqualizerModel.stepG(index, +0.2)
        onValueDown: EqualizerModel.stepG(index, -0.2)
    }
    Rectangle {
        implicitWidth: 96
        implicitHeight: 1
        color: Material.backgroundColor
    }
}
}
