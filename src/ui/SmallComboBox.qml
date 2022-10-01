import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQml.Models 2.12

ComboBox {
    property int modelWidth
    property var sizeModel: null

    id: control
    implicitWidth: modelWidth + 36
    implicitHeight: 24
    topInset: 0
    bottomInset: 0
    leftPadding: 6
    rightPadding: 6
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
        rightPadding: 6
        font: control.font
        text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
        Material.foreground: control.currentIndex === index ? ListView.view.contentItem.Material.accent : ListView.view.contentItem.Material.foreground
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
    }

    contentItem: Text {
        leftPadding: 0
        rightPadding: 15

        text: control.displayText
        font: control.font
        color: Material.foreground
        verticalAlignment: Text.AlignVCenter
        //horizontalAlignment: Text.AlignRight
        elide: Text.ElideRight
    }

    indicator: Canvas {
            id: canvas
            x: control.width - width - control.rightPadding
            y: control.topPadding + (control.availableHeight - height) / 2
            width: 12
            height: 6
            contextType: "2d"

            Connections {
                target: control
                function onPressedChanged() { canvas.requestPaint(); }
            }

            onPaint: {
                context.reset();
                context.moveTo(2, 0);
                context.lineTo(width-2, 0);
                context.lineTo(width / 2, height);
                context.closePath();
                context.fillStyle = Material.foreground
                context.fill();
            }
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
        //console.log("ComboBox> model size: ", model.length , ", new metrics: ", textMetrics.text, " width: ", textMetrics.width)
    }
}
