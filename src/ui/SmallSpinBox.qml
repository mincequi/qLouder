import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

SpinBox {
    property string unit: "dB"

    id: control
    topPadding: 0
    bottomPadding: 0
    editable: false
    implicitHeight: 24
    implicitWidth: 90
    font.pixelSize: 12
    font.bold: true
    textFromValue: function(value, locale) {
        return qsTr("%1 %2").arg(value).arg(unit);
    }

    contentItem: TextInput {
        text: control.displayText
        font: control.font
        color: enabled ? control.Material.foreground : control.Material.hintTextColor
        selectionColor: control.Material.textSelectionColor
        selectedTextColor: control.Material.foreground
        horizontalAlignment: Qt.AlignRight
        verticalAlignment: Qt.AlignVCenter
        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: control.inputMethodHints
    }

    background: Item {
        implicitWidth: 160
        implicitHeight: control.Material.touchTarget
    }
}
