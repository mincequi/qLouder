import QtQuick 2.0
import QtQuick.Templates 2.12 as T
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

T.RangeSlider {
    id: control
    property double progress: -1.0
    property real handleSize: 9.0

    signal progressHandleChanged(real progress)

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            first.implicitHandleWidth + leftPadding + rightPadding,
                            second.implicitHandleWidth + leftPadding + rightPadding)
    implicitHeight: 24
    padding: 6

    // progress handle
    Rectangle {
        id: progressHandle
        visible: control.progress > 0.0
        x: dragArea.drag.active ? x : first.handle.x + (handleSize-width)/2 + (second.handle.x - first.handle.x) * control.progress
        y: control.topPadding + (control.availableHeight - height) / 2
        z: 50
        radius: 3
        implicitWidth: 6
        implicitHeight: 6
        color: control.Material.accentColor

        Drag.active: dragArea.drag.active
        Drag.hotSpot.x: 3
        Drag.hotSpot.y: 3

        onXChanged: {
            if (dragArea.drag.active) {
                //control.progressHandleChanged((progressHandle.x - first.handle.x)/ (first.handle.x + (handleSize-width)/2 + (second.handle.x - first.handle.x)))
                var progress_ = (progressHandle.x - (width + control.handleSize)/2) / (control.availableWidth - control.handleSize) * (control.to - control.from)
                //if (progressHandle.x > second.handle.x - (control.handleSize - width)/2) {
                //    dragArea.drag.active = false
                //}
                control.progressHandleChanged(progress_) // Math.max(progress_, second.handle.x + (control.handleSize - width)/2))
            }
        }

        MouseArea {
            id: dragArea
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAxis
            drag.threshold: 1
            drag.minimumX: first.handle.x
            drag.maximumX: second.handle.x + (control.handleSize - progressHandle.width) / 2
        }
    }

    // progress indicator
    Rectangle {
        x: first.handle.x + control.handleSize/2
        y: (control.implicitHeight - height) / 2
        visible: control.progress > 0.0
        width: (second.handle.x - first.handle.x) * control.progress
        height: 2
        color: control.Material.accentColor
    }

    first.handle: Rectangle {
        x: control.leftPadding + (control.horizontal ? control.first.visualPosition * (control.availableWidth - width) : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : control.first.visualPosition * (control.availableHeight - height))
        z: 10
        implicitWidth: control.handleSize
        implicitHeight: control.handleSize
        radius: 9
        color: control.Material.primary //control.enabled ? control.Material.foreground : control.Material.sliderDisabledColor
        border.color: control.enabled ? control.Material.foreground : control.Material.sliderDisabledColor
    }
    second.handle: Rectangle {
        x: control.leftPadding + (control.horizontal ? control.second.visualPosition * (control.availableWidth - width) : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : control.second.visualPosition * (control.availableHeight - height))
        z: 10
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
