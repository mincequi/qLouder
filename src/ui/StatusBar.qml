import QtQuick 2.6
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import CrossoverModel 1.0
import StatusModel 1.0

ToolBar {
    height: 48
    width: parent.width
    Material.elevation: 0

    RowLayout {
        anchors.left: parent.left
        width: parent.width
        height: parent.height
        layoutDirection: "LeftToRight"
        spacing: 0

        TextEdit {
            //width: parent.width
            Layout.fillWidth: true
            height: parent.height
            readOnly: true
            padding: 6
            color: Material.primaryTextColor
            wrapMode: TextEdit.Wrap
            leftPadding: 12
            //horizontalAlignment: Qt.AlignRight
            //verticalAlignment: Qt.AlignVCenter
            text: StatusModel.text
            font.pixelSize: 10
        }

        // Spacer item
        /*
        Item {
            Layout.fillWidth: true
        }*/

        Label {
            height: 24
            leftPadding: 6
            topPadding: 3
            bottomPadding: 3
            text: StatusModel.propertyTitle
            font.bold: true
            font.pixelSize: 12
        }

        UpDownSpinBox {
            visible: StatusModel.tabIndex > 1
            Layout.maximumWidth: 84
            label: "Min"
            value: StatusModel.sumMin.toFixed(1)
            unit: "dB"
            valueColor: Material.accent
            buttonsVisible: false
        }
        UpDownSpinBox {
            visible: StatusModel.tabIndex > 1
            Layout.maximumWidth: 84
            label: "Max"
            value: StatusModel.sumMax.toFixed(1)
            unit: "dB"
            valueColor: Material.accent
            buttonsVisible: false
        }
        UpDownSpinBox {
            visible: StatusModel.ripple.length > 0
            Layout.maximumWidth: 84
            label: "Ripple"
            value: StatusModel.ripple
            unit: "dB"
            valueColor: Material.accent
            buttonsVisible: false
        }
    }
}
