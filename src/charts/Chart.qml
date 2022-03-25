import QtQuick 2.15
import QtCharts 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import ".."

import ChartModel 1.0

RowLayout {
    id: control
    property int type: ChartModel.ImpulseResponse
    property var model
    property alias chart: chart
    property list<Item> toolBarChildren
    property color axisLabelColor: Material.primary
    property color accentColor: Material.accent
    property color foregroundColor: Material.foreground

    property alias xAxis: xAxis
    property alias yAxis: yAxis
    property alias xAxisLog: xAxisLog

    layoutDirection: Qt.RightToLeft
    Layout.fillWidth: true
    Layout.fillHeight: true

    ToolBar {
        id: toolBar
        Layout.fillHeight: true
        width: 96
        Material.elevation: 0
        anchors.margins: 0
        anchors.top: parent.top
        anchors.right: parent.right

        ColumnLayout {
            id: toolBarLayout
            anchors.fill: parent
            spacing: 0

            children: toolBarChildren
        }
    }

    ChartView {
        id: chart
        antialiasing: true
        legend.visible: false

        backgroundRoundness: 0
        backgroundColor: Material.background

        margins.left: 0
        margins.right: 0
        margins.top: 0
        margins.bottom: 0

        Layout.fillWidth: true
        Layout.fillHeight: true

        // Setting the plotArea explicitly does not show the chart right away
        // It has to be resized before
        //plotArea: Qt.rect(48, 24, width-96, height-48)

        Component.onCompleted: {
            model.setSeries(linSeries)
            model.setIrWindowSeries(irWindowSeries)
            model.setUpperEnvelopeSeries(envelope)
            //model.setLowerEnvelopeSeries(chart.series(3))
            model.setIrWindowHandles(irWindowHandles)
        }

        // https://stackoverflow.com/questions/48789849/draging-a-point-on-qtcharts-in-qml
        MouseArea {
            id: mouseArea
            x: chart.x
            y: chart.y
            width: chart.width
            height: chart.height
            hoverEnabled: true
            property real tolerance: 0.05
            property var selectedPoint: undefined
            property int selectedHandle: -1

            onPressed: {
                //console.log("mouse> x: ", mouse.x, ", y: ", mouse.y)

                var cp = Qt.point(mouse.x,mouse.y);
                for (var i = 0; i < irWindowHandles.count; ++i) {
                    var p = chart.mapToPosition(irWindowHandles.at(i));
                    //console.log("p", i, "> x: ", p.x, ", y: ", p.y)

                    if (Math.abs(cp.x - p.x) <= irWindowHandles.markerSize/2 &&
                            Math.abs(cp.y - p.y) <= irWindowHandles.markerSize/2) {
                        mouseArea.selectedPoint = p;
                        mouseArea.selectedHandle = i;
                        console.log("onPressed> x: ", p.x, ", y: ", p.y)
                        break;
                    }
                }
            }

            onPositionChanged: {
                //console.log("mouse x: ", mouse.x, ", y: ", mouse.y)

                if (mouseArea.selectedPoint != undefined) {
                    var p = Qt.point(mouse.x, mouse.y);
                    var cp = chart.mapToValue(p);
                    if (cp.x >= xAxis.min && cp.x <= xAxis.max && cp.y >= yAxis.min && cp.y <= yAxis.max) {
                        //irWindowHandles.replace(mouseArea.selectedPoint.x, mouseArea.selectedPoint.y, cp.x, cp.y);
                        mouseArea.selectedPoint = cp;
                        model.moveIrHandle(mouseArea.selectedHandle, cp.x)
                        irWindowHandles.at(mouseArea.selectedPoint).x = p.x
                        console.log("onPositionChanged> x: ", cp.x, ", y: ", cp.y)
                    }
                }
            }

            onReleased: {
                mouseArea.selectedPoint = undefined;
                mouseArea.selectedHandle = -1;
                console.log("onReleased>")
            }
        }

        ValueAxis {
            id: xAxis
            visible: type == ChartModel.ImpulseResponse
            min: model.xMin
            max: model.xMax
            gridLineColor: control.axisLabelColor
            lineVisible: false
            tickInterval: model.tickInterval
            tickType: ValueAxis.TicksDynamic
            labelFormat: "%.0f"
            labelsFont.pointSize: 10
            labelsFont.bold: false
        }

        LogValueAxis {
            id: xAxisLog
            visible: type == ChartModel.FrequencyResponse
            base: 10
            min: model.xMin
            max: model.xMax
            gridLineColor: control.axisLabelColor
            lineVisible: false
            minorTickCount: -1
            minorGridLineColor: gridLineColor
            minorGridLinePen: gridLinePen
            labelFormat: "%d"
            labelsFont.pointSize: 10
            labelsFont.bold: false
        }

        ValueAxis {
            id: yAxis
            min: model.yMin
            max: model.yMax
            gridLineColor: control.axisLabelColor
            lineVisible: false
            minorTickCount: 1
            minorGridLineColor: gridLineColor
            minorGridLinePen: gridLinePen
            tickType: ValueAxis.TicksDynamic
            tickAnchor: 0.0
            tickInterval: 12.0
            labelsColor: control.axisLabelColor
            //labelFormat: "%2.1f"
            labelsFont.pointSize: 10
            labelsFont.bold: false
        }

        LineSeries {
            id: linSeries
            name: "Linear"
            axisX: xAxis
            axisY: yAxis
            width: 1.0
            color: control.accentColor
            opacity: 0.25
            visible: false
        }

        /*
        LineSeries {
            id: logSeries
            name: "Logarithmic"
            axisX: xAxisLog
            axisY: yAxis
            width: 1.0
            color: control.accentColor
        }
        */

        LineSeries {
            id: irWindowSeries
            name: "Linear"
            axisX: xAxis
            axisY: yAxis
            width: 1.0
            color: control.foregroundColor
            opacity: 1.0
            style: Qt.SolidLine

            onHovered: {
                //width: 3.0
            }
        }

        LineSeries {
            id: envelope
            name: "upperEnvelope"
            axisX: xAxis
            axisY: yAxis
            width: 1.0
            color: control.accentColor
        }

        LineSeries {
            name: "lowerEnvelope"
            axisX: xAxis
            axisY: yAxis
            width: 1.0
            color: control.accentColor
        }

        // TODO: move this to IrChart.qml
        ScatterSeries {
            id: irWindowHandles
            axisX: xAxis
            axisY: yAxis
            color: "transparent"
            visible: model.hasMeasurement && control.type == ChartModel.ImpulseResponse
            borderWidth: 1.0
            borderColor: control.foregroundColor
            markerSize: 9.0
            //markerShape: ScatterSeries.MarkerShapeRectangle
            XYPoint { x: model.irWindowLeft; y: 0.0 }
            XYPoint { x: model.irWindowRight; y: 0.0 }
        }
    }
}
