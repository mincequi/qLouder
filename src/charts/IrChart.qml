import QtQuick 2.0
import QtCharts 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import ".."

import ChartModel 1.0
import IrChartModel 1.0

Chart {
    id: chart
    type: ChartModel.ImpulseResponse

    model: IrChartModel {
        id: model
        type: chart.type
    }

    Binding {
        target: chart.handles
        property: "visible"
        value: model.hasMeasurement
    }

    Component.onCompleted: {
        chart.handles.color = "transparent"
        chart.handles.borderWidth = 1.0
        chart.handles.borderColor = chart.foregroundColor
        chart.handles.markerSize = 9.0
        chart.handles.append(model.irWindowLeft, 0.0)
        chart.handles.append(model.irWindowRight, 0.0)
        model.setHandles(chart.handles)
    }

    toolBarChildren: [
        RowLayout {
            spacing: 0
            Layout.topMargin: 6

            SmallToolButton {
                iconName: "magnify-minus"
                flat: true
                onClicked: model.zoom(0.5);
            }
            SmallToolButton {
                id: zoomIn
                iconName: "magnify-plus"
                flat: true
                onClicked: model.zoom(2.0);
            }
            SmallToolButton {
                iconName: "chevron-left"
                flat: true
                onClicked: model.pan(-0.2);
            }
            SmallToolButton {
                iconName: "chevron-right"
                flat: true
                onClicked: model.pan(0.2);
            }
        },

        ToolSeparator {
            anchors.left: parent.left
            anchors.right: parent.right
            topPadding: 6
            bottomPadding: 6
            orientation: Qt.Horizontal
        },

        Label {
            height: 24
            text: "Left window"
            leftPadding: 3
            font.pointSize: 12
        },
        RowLayout {
            spacing: 0
            Layout.leftMargin: 0
            Layout.rightMargin: 0
            enabled: model.hasMeasurement

            SmallToolButton {
                iconName: "minus"
                iconColor: Material.foreground
                onClicked: model.moveIrWindowLeft(-1);
            }

            Label {
                height: 24
                text: model.irWindowLeftReadout
                horizontalAlignment: Qt.AlignRight
                font.pointSize: 12
                font.bold: true
                Layout.fillWidth: true
            }

            SmallToolButton {
                iconName: "plus"
                iconColor: Material.foreground
                onClicked: model.moveIrWindowLeft(1);
            }
        },

        ToolSeparator {
            anchors.left: parent.left
            anchors.right: parent.right
            topPadding: 6
            bottomPadding: 6
            orientation: Qt.Horizontal
        },

        Label {
            height: 24
            text: "Right window"
            leftPadding: 3
            font.pointSize: 12
        },
        RowLayout {
            spacing: 0
            Layout.leftMargin: 0
            Layout.rightMargin: 0
            enabled: model.hasMeasurement

            SmallToolButton {
                iconName: "minus"
                iconColor: Material.foreground
                onClicked: model.moveIrWindowRight(-1);
            }

            Label {
                height: 24
                text: model.irWindowRightReadout
                horizontalAlignment: Qt.AlignRight
                font.pointSize: 12
                font.bold: true
                Layout.fillWidth: true
            }

            SmallToolButton {
                iconName: "plus"
                iconColor: Material.foreground
                onClicked: model.moveIrWindowRight(1);
            }
        },

        // spacer item
        Item {
            Layout.fillHeight: true
        }
    ]
}
