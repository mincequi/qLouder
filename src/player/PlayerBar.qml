import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.12
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15

import PlayerModel 1.0

import ".."

ToolBar {
    height: 24
    Material.elevation: 0

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6

        SmallToolButton {
            iconName: "file-music"
            flat: true
            onClicked: {
                fileDialog.open()
            }
        }

        SmallToolButton {
            iconName: PlayerModel.isPlaying ? "pause" : "play"
            flat: true
            onClicked: PlayerModel.togglePlayPause()
        }

        SmallRangeSlider {
            Layout.fillWidth: true
            progress: PlayerModel.progress
            first.value: PlayerModel.begin
            second.value: PlayerModel.end
            first.onVisualPositionChanged: PlayerModel.setBegin(first.value)
            second.onVisualPositionChanged: PlayerModel.setEnd(second.value)
        }
    }

    FileDialog {
        id: fileDialog
        title: "Music file"
        folder: shortcuts.music
        nameFilters: PlayerModel.supportedFileExtensions
        onAccepted: {
            var path = fileDialog.fileUrl.toString();
            // remove prefixed "file:///"
            path = path.replace(/^(file:\/{3})/,"/");
            // unescape html codes like '%23' for '#'
            var cleanPath = decodeURIComponent(path);
            PlayerModel.setFile(cleanPath)
        }
    }
}
