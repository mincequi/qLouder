import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

ToolButton {
    property alias iconName: icon.name
    property color iconColor: control.Material.foreground
    property alias iconRotation: icon.rotation
    property alias iconEnabled: icon.enabled
    property alias iconX: icon.x
    property alias iconY: icon.y
    property real yOffset: 0

    id: control
    autoRepeat: true
    implicitHeight: 24
    implicitWidth: text.length ? textMetrics.width + 36 : 24
    //leftPadding: (availableWidth - textMetrics.width)/2
    rightPadding: (implicitWidth - textMetrics.width) - height*2
    font.pixelSize: 12
    font.capitalization: Font.MixedCase
    spacing: 0

    TextMetrics {
        id: textMetrics
        font: control.font
        text: control.text
    }

    MaterialDesignIcon {
        id: icon
        x: text.length ? (control.height - size)/2 : (control.width - size)/2
        y: (control.height - size)/2 + control.yOffset
        color: control.iconEnabled ? control.iconColor : control.Material.hintTextColor
    }
}
