import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import ProjectsModel 1.0

ColumnLayout {
    id: item
    width: 96
    property var projectModel
    property int itemHeight: 18

    spacing: 0
    RowLayout {
        spacing: 0
        Label {
            id: label
            text: "Project " + (index + 1).toString()
            height: itemHeight
            leftPadding: 6
            font.pixelSize: 12
        }
        Item {
            Layout.fillWidth: true
        }
        SmallToolButton {
            id: deleteFilter
            implicitHeight: itemHeight
            iconName: "close"
            iconColor: Material.color(Material.Red, Material.Shade300)
            flat: true
            onClicked: {
                ProjectsModel.removeProject(index)
            }
        }
    }

    ToolSeparator {
        Layout.fillWidth: true
        topPadding: 0
        bottomPadding: 0
        orientation: Qt.Horizontal
    }
}
