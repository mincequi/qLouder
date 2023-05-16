import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import ProjectsModel 1.0

Rectangle {
    property var chart2
    property var projectModel
    id: control
    implicitWidth: 96
    implicitHeight: 18
    color: Material.primary
    //radius: 6.0
    ColumnLayout {
        id: item
        width: 96
        property int itemHeight: 18

        spacing: 0
        RowLayout {
            spacing: 0
            Label {
                id: label
                text: control.projectModel.name
                elide: Text.ElideLeft
                height: item.itemHeight
                Layout.preferredWidth: 72
                leftPadding: 6
                font.pixelSize: 10
            }
            Item {
                Layout.fillWidth: true
            }
            MicroToolButton {
                id: deleteFilter
                implicitHeight: item.itemHeight
                iconName: "close"
                iconColor: Material.color(Material.Red, Material.Shade300)
                flat: true
                onClicked: {
                    ProjectsModel.removeProject(index)
                }
            }
        }

        Rectangle {
            implicitWidth: 96
            implicitHeight: 1
            color: Material.backgroundColor
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: { ProjectsModel.setIndex(index) }
    }
}
