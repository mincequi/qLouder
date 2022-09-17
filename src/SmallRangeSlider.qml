import QtQuick 2.0
import QtQuick.Templates 2.15 as T
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15

T.RangeSlider {
    id: control
    property double progress: -1.0
    property real handleSize: 9.0

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            first.implicitHandleWidth + leftPadding + rightPadding,
                            second.implicitHandleWidth + leftPadding + rightPadding)
    implicitHeight: 24
    padding: 6

    // progress indicator
    /*
    Rectangle {
        visible: control.progress >= 0.0
        x: control.leftPadding + control.progress * (control.availableWidth - width)
        y: control.topPadding + (control.availableHeight - height) / 2
        z: 50
        implicitWidth: 2
        implicitHeight: 9
        color: control.Material.accentColor
    }
    */

    Rectangle {
        x: first.handle.x + control.handleSize
        y: (control.implicitHeight - height) / 2
        z: 50
        visible: control.progress > 0.0
        width: (second.handle.x - control.handleSize - first.handle.x) * control.progress
        height: 2
        color: control.Material.accentColor
    }

    first.handle: Rectangle {
        x: control.leftPadding + (control.horizontal ? control.first.visualPosition * (control.availableWidth - width) : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : control.first.visualPosition * (control.availableHeight - height))
        implicitWidth: control.handleSize
        implicitHeight: control.handleSize
        radius: 9
        color: control.Material.primary //control.enabled ? control.Material.foreground : control.Material.sliderDisabledColor
        border.color: control.enabled ? control.Material.foreground : control.Material.sliderDisabledColor
    }
    second.handle: Rectangle {
        x: control.leftPadding + (control.horizontal ? control.second.visualPosition * (control.availableWidth - width) : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : control.second.visualPosition * (control.availableHeight - height))
        implicitWidth: control.handleSize
        implicitHeight: control.handleSize
        radius: 9
        color: control.Material.primary //control.enabled ? control.Material.foreground : control.Material.sliderDisabledColor
        border.color: control.enabled ? control.Material.foreground : control.Material.sliderDisabledColor
    }
    background: Rectangle {
        x: control.leftPadding + (control.horizontal ? 0 : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : 0)
        implicitWidth: control.horizontal ? 200 : 48
        implicitHeight: control.horizontal ? 48 : 200
        width: control.horizontal ? control.availableWidth : 2
        height: control.horizontal ? 2 : control.availableHeight
        scale: control.horizontal && control.mirrored ? -1 : 1
        color: control.enabled ? Color.transparent(control.Material.foreground, 0.33) : control.Material.sliderDisabledColor
        Rectangle {
            x: control.horizontal ? control.first.position * parent.width : 0
            y: control.horizontal ? 0 : control.second.visualPosition * parent.height
            width: control.horizontal ? control.second.position * parent.width - control.first.position * parent.width : 2
            height: control.horizontal ? 2 : control.second.position * parent.height - control.first.position * parent.height
            color: control.enabled ? control.Material.foreground : control.Material.sliderDisabledColor
        }
    }
}
