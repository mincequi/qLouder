import QtQuick 2.4
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import ExportModel 1.0

Page {
    header: Row {
        height: 24
        width: parent.width
        ToolBar {
            height: parent.height
            width: parent.width
            Material.elevation: 0
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 12
        anchors.leftMargin: 12
        anchors.rightMargin: 12

        GridLayout {
            columns: 1
            columnSpacing: 12

            Label {
                enabled: ExportModel.isPulseEffectsAvailable
                text: "PulseEffects exporter"
                font.pixelSize: 12
            }
            SmallToolButton {
                Layout.leftMargin: 12
                implicitWidth: 96
                enabled: ExportModel.isPulseEffectsAvailable
                iconName: "export"
                text: "Export..."
                onClicked: {
                    ExportModel.exportToPulseEffects()
                }
            }
            SmallSwitch {
                enabled: ExportModel.isPulseEffectsAvailable
                Layout.leftMargin: 12
                text: "Bind"
                implicitWidth: 96
                onCheckedChanged: ExportModel.bindToPulseEffects(checked)
            }

            /*
            SmallToolButton {
                //implicitWidth: 84
                enabled: ExportModel.isPulseEffectsAvailable
                iconName: ExportModel.isPulseEffectsBound ? "lock" : "lock-open"
                text: ExportModel.isPulseEffectsBound ? "Bound" : "Bind"
                onClicked: {
                    ExportModel.togglePulseEffectsBind()
                }
            }
            */

            Item {
                Layout.columnSpan: 3
                Layout.fillHeight: true
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Export file"
        folder: shortcuts.home
        selectExisting: true
        selectMultiple: false
        onAccepted: {
            var path = fileDialog.fileUrl.toString();
            // remove prefixed "file:///"
            path = path.replace(/^(file:\/{3})/,"/");
            // unescape html codes like '%23' for '#'
            var cleanPath = decodeURIComponent(path);
            ExportModel.exportPulseEffects(cleanPath)
        }
    }
}
