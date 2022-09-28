import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.12
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15

import MeasureModel 1.0
import PlayerModel 1.0
import PlayerBarModel 1.0

ToolBar {
    height: 48
    Material.elevation: 0

    SmallRangeSlider {
        id: sineSweepRangeSlider
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: 6
        anchors.right: parent.right
        anchors.rightMargin: 6
        visible: !PlayerModel.isPlaying
        from: 0
        // We cannot dynamically change "to:" parameter. So, we need two instances of RangeSlider
        to: MeasureModel.frequencies.length - 1
        snapMode: RangeSlider.SnapAlways
        stepSize: 1
        first.value: MeasureModel.minFrequencySlider
        second.value: MeasureModel.maxFrequencySlider
        first.onMoved: MeasureModel.setMinFrequencySlider(first.value)
        second.onMoved: MeasureModel.setMaxFrequencySlider(second.value)
        progress: MeasureModel.progress
    }

    SmallRangeSlider {
        id: sampleRangeSlider
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: 6
        anchors.right: parent.right
        anchors.rightMargin: 6
        visible: PlayerModel.isPlaying
        from: 0
        to: PlayerModel.totalTime
        first.value: PlayerModel.loopBeginTime
        second.value: PlayerModel.loopEndTime
        first.onMoved: PlayerModel.setBegin(first.value)
        second.onMoved: PlayerModel.setEnd(second.value)
        progress: PlayerModel.progress
    }

    Label {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 12
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        height: 24
        text: PlayerBarModel.leftReadout
        font.pixelSize: 12
        font.bold: true
        Layout.minimumHeight: 24
    }

    Label {
        id: rightReadout
        height: 24
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 12
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
        Layout.fillHeight: true
        text: PlayerBarModel.rightReadout
        font.pixelSize: 12
        font.bold: true
    }

    SmallToolButton {
        id: deleteFilter
        visible: PlayerModel.isPlaying
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 72
        iconName: "tune-vertical"
        iconColor: PlayerModel.isEqualizerEnabled ? Material.accent : Material.foreground
        flat: true
        onClicked: {
            PlayerModel.toggleEqualizer()
        }
    }

    Label {
        height: 24
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        Layout.fillWidth: true
        Layout.fillHeight: true
        text: PlayerBarModel.title
        font.pixelSize: 12
    }
}
