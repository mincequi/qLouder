import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

ToolButton {
    property alias iconName: icon.name
    property alias iconColor: icon.color
    property alias iconRotation: icon.rotation
    property alias iconEnabled: icon.enabled

    id: control
    implicitHeight: 24
    implicitWidth: text.length ? textMetrics.width + 36 : 24
    leftPadding: icon.size
    rightPadding: 0
    font.pointSize: 12
    font.capitalization: Font.MixedCase
    spacing: 0

    TextMetrics {
        id: textMetrics
        font: control.font
        text: control.text
    }

    MaterialDesignIcon {
        id: icon
        x: (control.height - size)/2
        y: (control.height - size)/2
        color: !control.iconEnabled ? control.Material.hintTextColor : control.down || control.checked ? control.Material.accentColor : control.Material.foreground
    }
}
