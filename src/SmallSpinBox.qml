import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

SpinBox {
    id: control
    topPadding: 0
    bottomPadding: 0
    editable: false
    implicitHeight: 24
    implicitWidth: 90
    font.pointSize: 12
    font.bold: true
    textFromValue: function(value, locale) {
        return qsTr("%1 dB").arg(value);
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
