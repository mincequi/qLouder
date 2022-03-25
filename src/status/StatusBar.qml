import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.12

import StatusModel 1.0

ToolBar {
    height: 24
    width: parent.width
    Material.elevation: 0

    Label {
        leftPadding: 12
        height: 24
        verticalAlignment: Qt.AlignVCenter
        text: StatusModel.text
        font.pointSize: 12
    }
}
