import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

TabButton {
    property alias materialIcon: icon.name

    id: control

    width: 72
    implicitHeight: 72  // must override implicitHeight. If overriding height, parent layout collapses

    contentItem: Item {
        MaterialDesignIcon {
            id: icon
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            size: 32
            name: control.icon.name
            color: !control.enabled ? control.Material.hintTextColor : control.down || control.checked ? control.Material.accentColor : control.Material.foreground
        }
        Label {
            id: label
            text: control.text
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            font.bold: true
            font.pointSize: 12
            color: !control.enabled ? control.Material.hintTextColor : control.down || control.checked ? control.Material.accentColor : control.Material.foreground
        }
    }
}
