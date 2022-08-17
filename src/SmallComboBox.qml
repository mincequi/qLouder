import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQml.Models 2.15

ComboBox {
    property int modelWidth
    property var sizeModel: null

    id: control
    implicitWidth: modelWidth + 45
    implicitHeight: 24
    topInset: 0
    bottomInset: 0
    padding: -6
    flat: false

    font.pixelSize: 12
    font.bold: true

    popup.width: control.implicitWidth
    popup.topMargin: 0

    delegate: MenuItem {
        width: ListView.view.width
        height: 24.0001
        topInset: 0
        leftPadding: 6
        font: control.font
        text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
        Material.foreground: control.currentIndex === index ? ListView.view.contentItem.Material.accent : ListView.view.contentItem.Material.foreground
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
    }

    TextMetrics {
        id: textMetrics
        font: control.font
        text: "MacBook Air Microphone"
    }
    onModelChanged: {
        for(var i = 0; i < model.length; i++){
            textMetrics.text = model[i]
            modelWidth = Math.max(textMetrics.width, modelWidth)
        }
        if (sizeModel != null) {
            for(var j = 0; j < sizeModel.length; j++){
                textMetrics.text = sizeModel[j]
                modelWidth = Math.max(textMetrics.width, modelWidth)
            }
        }
        //console.log("New metrics: ", textMetrics.text, " width: ", textMetrics.width)
    }
}
