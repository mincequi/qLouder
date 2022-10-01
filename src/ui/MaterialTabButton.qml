import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

TabButton {
    property alias materialIcon: icon.name
    property alias iconSize: icon.size
    property bool horizontal: availableWidth > availableHeight * 1.5

    id: control

    width: 72
    implicitHeight: 60  // must override implicitHeight. If overriding height, parent layout collapses

    padding: 0

    contentItem: Item {
        MaterialDesignIcon {
            id: icon
            x: control.horizontal ? (control.availableHeight - size) / 2 : (control.availableWidth - size) / 2
            y: control.horizontal ? (control.availableHeight - size) / 2 : (control.availableHeight - size - label.height) / 2
            size: 32
            name: control.icon.name
            color: !control.enabled ? control.Material.hintTextColor : control.down || control.checked ? control.Material.accentColor : control.Material.foreground
        }
        Label {
            id: label
            text: control.text
            anchors.left: parent.left
            anchors.leftMargin: control.horizontal ? control.availableHeight : (control.availableWidth - width) / 2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: label.height / 2
            font.bold: true
            font.pixelSize: 10
            color: !control.enabled ? control.Material.hintTextColor : control.down || control.checked ? control.Material.accentColor : control.Material.foreground
        }
    }
}
