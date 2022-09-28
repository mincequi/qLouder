import QtQuick 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Shapes 1.15

Item {
    id: item
    property var radius: 64
    property var color: Material.color(Material.Blue, Material.Shade500)    
    property color backgroundColor: "transparent"

    property var size: 8

    Behavior on rotation { SmoothedAnimation { duration: gauge.animationDuration; velocity: -1; easing.type: gauge.easingType } }

    Shape {
        ShapePath {
            strokeWidth: 0
            strokeColor: item.backgroundColor
            fillColor: item.color
            startY: item.radius
            PathLine { relativeX: size/2; relativeY: size }
            PathLine { relativeX: -size; relativeY: 0 }
            PathLine { relativeX: size/2; relativeY: -size }
        }
    }
}
