import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.12

TabBar {
    id: control
    width: parent.width
    background: Rectangle {
        color: Material.primary
    }

    contentItem: ListView {
        width: control.width
        height: control.height
        model: control.contentModel
        currentIndex: control.currentIndex

        spacing: control.spacing
        orientation: ListView.Vertical
        boundsBehavior: Flickable.StopAtBounds
    }
}
