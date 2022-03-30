import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Templates 2.15 as T

T.Switch {
    id: control
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: 24
    padding: 0
    spacing: 0
    /*
    indicator: SwitchIndicator {
        implicitHeight: 24
        x: control.leftPadding + (control.availableWidth - width) - 6
        y: control.topPadding + (control.availableHeight - height) / 2
        control: control
        Ripple {
            x: parent.handle.x + parent.handle.width / 2 - width / 2
            y: parent.handle.y + parent.handle.height / 2 - height / 2
            width: 24; height: 24
            pressed: control.pressed
            active: control.down || control.visualFocus || control.hovered
            color: control.checked ? control.Material.highlightedRippleColor : control.Material.rippleColor
        }
    }
    */

    indicator: Item {
        id: indicator
        implicitWidth: 24
        implicitHeight: 24
        x: control.leftPadding + (control.availableWidth - width) - 6
        y: control.topPadding + (control.availableHeight - height) / 2
        property Item control
        property alias handle: handle
        Material.elevation: 1
        Rectangle {
            width: parent.width
            height: 9
            radius: height / 2
            y: parent.height / 2 - height / 2
            color: control.enabled ? (control.checked ? control.Material.switchCheckedTrackColor : control.Material.switchUncheckedTrackColor)
                                   : control.Material.switchDisabledTrackColor
        }
        Rectangle {
            id: handle
            x: Math.max(0, Math.min(parent.width - width, control.visualPosition * parent.width - (width / 2)))
            y: (parent.height - height) / 2
            width: 15
            height: 15
            radius: width / 2
            color: control.enabled ? (control.checked ? control.Material.switchCheckedHandleColor : control.Material.switchUncheckedHandleColor)
                                   : control.Material.switchDisabledHandleColor
            Behavior on x {
                enabled: !control.pressed
                SmoothedAnimation {
                    duration: 300
                }
            }
            layer.enabled: Material.elevation > 0
            layer.effect: ElevationEffect {
                elevation: Material.elevation
            }
        }
        Ripple {
            x: parent.handle.x + parent.handle.width / 2 - width / 2
            y: parent.handle.y + parent.handle.height / 2 - height / 2
            width: 18; height: 18
            pressed: control.pressed
            active: control.down || control.visualFocus || control.hovered
            color: control.checked ? control.Material.highlightedRippleColor : control.Material.rippleColor
        }
    }

    Label {
        y: control.topPadding + (control.availableHeight - height) / 2
        font.pointSize: 12
        leftPadding: 6
        text: "Invert"
    }
}
