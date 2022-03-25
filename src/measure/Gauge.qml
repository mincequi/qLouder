import QtQuick 2.11
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.15
import QtQuick.Shapes 1.15

import QtGraphicalEffects 1.12

Item {
    id: gauge

    property real radius: 64
    property real strokeWidth: 12
    property real pointerSize: 15
    property real barMargin: 0
    property int barCount: 1

    property real value: 0.0
    property real valueWarning: 100.0
    property real valueMax: valueRangeMin
    property real valueRangeMin: -48.0
    property real valueRangeMax: 12.0
    property string unit: "dB"
    property real range: valueRangeMax - valueRangeMin
    property real angle: (Math.max(Math.min(value, valueRangeMax), valueRangeMin) - valueRangeMin)/range * 270.0
    property real angleMax: (Math.max(Math.min(valueMax, valueRangeMax), valueRangeMin) - valueRangeMin)/range * 270.0

    property color valueColor: value > valueWarning ? Material.color(Material.Red, Material.Shade300) : Material.accent
    property color valueMaxColor: valueMax > valueWarning ? Material.color(Material.Red, Material.Shade300) : Material.accent
    property color valueBackgroundColor: colorWithAlpha(Material.accent, 0.33)
    property color warningBackgroundColor: colorWithAlpha(Material.color(Material.Red, Material.Shade300), 0.5)
    property color backgroundColor: Material.background
    property color primaryColor: Material.primaryColor
    property int animationDuration: 250
    property int easingType: Easing.Linear

    //Layout.fillWidth: true
    //Layout.fillHeight: true
    implicitWidth: radius*2 + Math.max(gauge.strokeWidth, 2*gauge.pointerSize)
    implicitHeight: radius*2 + Math.max(gauge.strokeWidth, 2*gauge.pointerSize)
    //anchors.centerIn: parent
    //anchors.bottom: parent.bottom
    //anchors.right: parent.right
    // multisample, decide based on your scene settings
    layer.enabled: true
    layer.samples: 4

    function colorWithAlpha(color, alpha) {
        return Qt.rgba(color.r, color.g, color.b, alpha)
    }

    // Gauge 1
    Shape {
        id: gauge1
        ShapePath {
            fillColor: "transparent"
            strokeColor: valueBackgroundColor
            strokeWidth: gauge.strokeWidth
            capStyle: ShapePath.FlatCap

            PathAngleArc {
                centerX: width/2 //radius + strokeWidth/2
                centerY: width/2 //radius + strokeWidth/2
                radiusX: radius
                radiusY: radius
                startAngle: 90
                sweepAngle: -valueRangeMin/range*270.0
            }
        }
        ShapePath {
            fillColor: "transparent"
            strokeColor: warningBackgroundColor
            strokeWidth: gauge.strokeWidth
            capStyle: ShapePath.FlatCap

            PathAngleArc {
                centerX: width/2 //radius + strokeWidth/2
                centerY: width/2 //radius + strokeWidth/2
                radiusX: radius
                radiusY: radius
                startAngle: 90 -valueRangeMin/range*270.0
                sweepAngle: valueRangeMax/range*270.0
            }
        }

        ShapePath {
            fillColor: "transparent"
            strokeColor: valueColor
            strokeWidth: gauge.strokeWidth
            capStyle: ShapePath.FlatCap
            Material.elevation: 4

            PathAngleArc {
                centerX: width/2 //radius + strokeWidth/2
                centerY: width/2 //radius + strokeWidth/2
                radiusX: radius
                radiusY: radius
                startAngle: 90
                sweepAngle: angle

                Behavior on sweepAngle { SmoothedAnimation { duration: animationDuration; velocity: -1; easing.type: easingType } }
            }
        }

        GaugePointer {
            x: gauge.width/2 // gauge.radius + strokeWidth/2
            y: gauge.width/2 //gauge.radius + strokeWidth/2
            size: pointerSize
            radius: gauge.radius
            color: valueMaxColor
            backgroundColor: gauge.backgroundColor
            rotation: angleMax
        }

        Text {
            x: gauge.radius + strokeWidth + gauge.pointerSize/2
            y: 2*gauge.radius + gauge.pointerSize/2 - 1
            //text: "Level"
            //anchors.bottom: gauge.bottom
            //anchors.baseline: parent.bottom
            font.pixelSize: 10
            color: valueColor
        }
    }

    // Gauge 2
    Shape {
        id: gauge2
        visible: barCount  > 1
        ShapePath {
            fillColor: "transparent"
            strokeColor: Material.color(Material.Grey, Material.Shade900)
            strokeWidth: gauge.strokeWidth
            capStyle: ShapePath.FlatCap

            PathAngleArc {
                centerX: width/2
                centerY: height/2
                radiusX: gauge.radius - gauge.strokeWidth - gauge.barMargin
                radiusY: gauge.radius - gauge.strokeWidth - gauge.barMargin
                startAngle: 90
                sweepAngle: 270
            }
        }

        ShapePath {
            fillColor: "transparent"
            strokeColor: Material.color(Material.Teal, Material.Shade500)
            strokeWidth: gauge.strokeWidth
            capStyle: ShapePath.FlatCap
            Material.elevation: 4

            PathAngleArc {
                centerX: width/2
                centerY: height/2
                radiusX: gauge.radius - gauge.strokeWidth - gauge.barMargin
                radiusY: gauge.radius - gauge.strokeWidth - gauge.barMargin
                startAngle: 90
                //sweepAngle: inverter.stringLiveData[1].power/inverter.stringLiveData[1].powerPeak*270.0

                Behavior on sweepAngle { SmoothedAnimation { duration: animationDuration; velocity: -1; easing.type: Easing.InOutElastic } }
            }
        }

        GaugePointer {
            x: gauge.width/2 // gauge.radius + strokeWidth/2
            y: gauge.width/2 //gauge.radius + strokeWidth/2
            size: -pointerSize
            radius: gauge.radius - gauge.strokeWidth - gauge.barMargin
            color: Material.color(Material.Teal, Material.Shade500)
            //rotation: inverter.stringLiveData[1].powerPeakToday/inverter.stringLiveData[1].powerPeak*270.0
        }

        Text {
            x: gauge.radius + strokeWidth + gauge.pointerSize/2
            y: 2*gauge.radius - strokeWidth - gauge.barMargin - 1 + gauge.pointerSize/2
            //text: inverter.stringLiveData[1].name
            font.pixelSize: 10
            color: Material.color(Material.Teal, Material.Shade500)
        }
    } // Shape

    // Text
    GridLayout {
        anchors.right: parent.right
        anchors.rightMargin: gauge.pointerSize/2
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: (gauge.radius + strokeWidth/2 - barCount * strokeWidth - (barCount-1)*barMargin)/2
        columns: 2
        rowSpacing: -6.0
        Text {
            Layout.alignment: Qt.AlignRight
            color: valueColor
            text: value.toFixed(0)
            font.bold: true
            font.pointSize: 30
        }
        Text {
            Layout.alignment: Qt.AlignLeft
            color: valueColor
            text: unit
            font.pointSize: 24
        }
        Text {
            id: readout2
            //visible: barCount > 1
            Layout.alignment: Qt.AlignRight
            color: valueMaxColor
            text: valueMax.toFixed(0)
            font.bold: true
            font.pointSize: 24
        }
        Text {
            id: unit2
            //visible: barCount > 1
            Layout.alignment: Qt.AlignLeft
            color: valueMaxColor
            text: unit
            font.pointSize: 18
        }
    }

} // Item
