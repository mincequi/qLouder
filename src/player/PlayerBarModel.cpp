#include "PlayerBarModel.h"

#include <measure/MeasureModel.h>
#include <player/PlayerModel.h>

#include <QTime>

PlayerBarModel::PlayerBarModel(MeasureModel& measurementModel,
                           PlayerModel& playerModel,
                           QObject *parent)
    : QObject{parent},
      _measurementModel(measurementModel),
      _playerModel(playerModel) {

    connect(&_measurementModel, &MeasureModel::measureStateChanged, this, &PlayerBarModel::stateChanged);
    connect(&_measurementModel, &MeasureModel::rangeChanged, this, &PlayerBarModel::stateChanged);
    connect(&playerModel, &PlayerModel::statusChanged, this, &PlayerBarModel::stateChanged);
}

bool PlayerBarModel::isPlaying() const {
    return _playerModel.isPlaying();
}

bool PlayerBarModel::isMeasuring() const {
    return _measurementModel.isMeasuring();
}

QString PlayerBarModel::leftReadout() const {
    if (isPlaying()) {
        return QTime::fromMSecsSinceStartOfDay(_playerModel.loopBeginTime() * 1000).toString("mm:ss");
    } else {
        return _measurementModel.minFrequencyReadout();
    }
}

QString PlayerBarModel::rightReadout() const {
    if (isPlaying()) {
        return QTime::fromMSecsSinceStartOfDay(_playerModel.loopEndTime() * 1000).toString("mm:ss");
    } else {
        return _measurementModel.maxFrequencyReadout();
    }
}

QString PlayerBarModel::title() const {
    if (isPlaying()) {
        return _playerModel.title();
    } else {
        return "Sine sweep";
    }
}

void PlayerBarModel::togglePlayPause() {
    _playerModel.togglePlayPause();
}

void PlayerBarModel::toggleMeasure() {
    _measurementModel.onMeasureButtonClicked();
}
