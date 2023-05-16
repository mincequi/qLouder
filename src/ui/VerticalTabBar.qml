import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

TabBar {
    id: control
    width: parent.width
    background: Rectangle {
        color: Material.primary
        //radius: 6.0
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
