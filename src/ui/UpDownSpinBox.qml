import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

import ".."

RowLayout {
    id: control
    spacing: 0
    Layout.leftMargin: 0
    Layout.rightMargin: 0

    property string label
    property string value
    property color valueColor: Material.foreground
    property bool buttonsVisible: true

    signal valueUp()
    signal valueDown()

    Label {
        id: label
        width: 24
        height: 24
        text: control.label
        leftPadding: 6
        font.pointSize: 12
    }

    Label {
        height: 24
        text: control.value
        color: control.valueColor
        horizontalAlignment: Qt.AlignRight
        Layout.rightMargin: buttonsVisible ? 0 : 6
        font.pointSize: 12
        font.bold: true
        Layout.fillWidth: true
    }

    ColumnLayout {
        visible: buttonsVisible
        spacing: 0
        SmallToolButton {
            iconY: -1
            iconName: "chevron-up"
            iconColor: Material.foreground
            implicitHeight: 12
            onClicked: control.valueUp()
        }
        SmallToolButton {
            iconY: -5
            iconName: "chevron-down"
            iconColor: Material.foreground
            implicitHeight: 12
            onClicked: control.valueDown()
        }
    }
}
