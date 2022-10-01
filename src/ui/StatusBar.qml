import QtQuick 2.6
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import StatusModel 1.0

ToolBar {
    height: 48
    width: parent.width
    Material.elevation: 0

    TextEdit {
        width: parent.width
        height: parent.height
        readOnly: true
        padding: 6
        color: Material.primaryTextColor
        wrapMode: TextEdit.Wrap
        //leftPadding: 12
        //horizontalAlignment: Qt.AlignRight
        //verticalAlignment: Qt.AlignVCenter
        text: StatusModel.text
        font.pixelSize: 10
    }
}
