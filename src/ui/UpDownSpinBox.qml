import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

RowLayout {
    id: control
    spacing: 0
    Layout.leftMargin: 0
    Layout.rightMargin: 0

    implicitHeight: 18

    property string label
    property string value
    property string unit
    property color valueColor: Material.foreground
    property bool buttonsVisible: true

    signal valueUp()
    signal valueDown()

    Label {
        id: label
        width: 24
        height: parent.height
        text: control.label
        leftPadding: 6
        font.pixelSize: 10
        Layout.alignment: Qt.AlignBaseline
    }

    Label {
        height: parent.height
        text: control.value
        horizontalAlignment: Qt.AlignRight
        Layout.rightMargin: unitLabel.visible ? 3 : 0
        font.pixelSize: 12
        font.bold: true
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignBaseline
    }

    Label {
        id: unitLabel
        height: parent.height
        visible: control.unit
        text: control.unit
        horizontalAlignment: Qt.AlignRight
        Layout.rightMargin: buttonsVisible ? 0 : 6
        font.pixelSize: 10
        Layout.alignment: Qt.AlignBaseline
    }

    ColumnLayout {
        visible: buttonsVisible
        spacing: 0
        SmallToolButton {
            yOffset: 1
            iconName: "chevron-up"
            iconColor: Material.foreground
            implicitHeight: control.implicitHeight/2
            onClicked: control.valueUp()
        }
        SmallToolButton {
            yOffset: -1
            iconName: "chevron-down"
            iconColor: Material.foreground
            implicitHeight: control.implicitHeight/2
            onClicked: control.valueDown()
        }
    }
}
