import QtQuick 2.0
import QtQuick.Controls.Material 2.12
import "MaterialDesignIconGlyphs.js" as MaterialGlyphs

Item {
    property int size: 18
    property string name
    property color color: Material.hintTextColor

    width: size
    height: size

    Text {
        anchors.fill: parent

        color: parent.color
        horizontalAlignment: Text.AlignHCenter

        font.family: materialFont.name
        font.pixelSize: parent.height

        text: MaterialGlyphs.glyphs[parent.name]
    }

    FontLoader {
        id: materialFont
        source: "qrc:/ui/materialdesignicons-webfont.ttf"
    }
}
