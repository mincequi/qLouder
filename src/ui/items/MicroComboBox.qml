import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Templates 2.12 as T
//import QtQuick.Controls.Material.impl 1.1

ComboBox {
    id: control

    implicitHeight: 24
    topInset: 0
    bottomInset: 0
    padding: 6
    rightPadding: 0
    font.pixelSize: 12
    font.bold: true
    flat: true

    onWidthChanged: {
        //console.log("width:", width)
    }

    contentItem: Text {
        leftPadding: 0
        rightPadding: 15

        text: control.displayText
        font: control.font
        color: Material.foreground
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    delegate: MenuItem {
        height: control.implicitHeight
        padding: 0
        verticalPadding: 0
        leftPadding: 6
        rightPadding: 6
        font: control.font
        text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
        //Material.foreground: control.currentIndex == index ? Material.accent : Material.foreground
        Material.foreground: control.currentIndex === index ? ListView.view.contentItem.Material.accent : ListView.view.contentItem.Material.foreground
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
    }

    indicator.x: control.width - 24 - (indicator.width - 24)/2
}
