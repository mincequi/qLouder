#include "PlayerModel.h"

#include <iostream>

#include <equalizer/EqualizerModel.h>
#include <equalizer/FilterModel.h>
#include <player/EqualizerNode.h>

#include "PlayerModelPrivate.h"

using namespace cinder::audio;

PlayerModel::PlayerModel(const EqualizerModel& equalizerModel, QObject *parent)
    : QObject{parent},
      _equalizerModel(equalizerModel),
      _d(new PlayerModelPrivate) {
    connect(&_timer, &QTimer::timeout, this, &PlayerModel::progressChanged);
    connect(&equalizerModel, &EqualizerModel::rangeChanged, this, &PlayerModel::onFiltersChanged);
}

PlayerModel::~PlayerModel() {
    delete _d;
}

QStringList PlayerModel::supportedFileExtensions() const {
    QString out;
    const auto exts =_d->supportedFileExtensions();
    for (const auto& e : exts) {
        out.append("*." + QString::fromStdString(e) + " ");
    }

     return { out };
}

bool PlayerModel::isPlaying() const {
    return _d->isPlaying();
}

double PlayerModel::begin() const {
    return _d->begin();
}

double PlayerModel::end() const {
    return _d->end();
}

double PlayerModel::progress() const {
    return _d->progress();
}

void PlayerModel::setFile(const QString& file) {
    if (_d->setFile(file.toStdString())) {
        emit loopChanged();
    }
}

void PlayerModel::setDevice(const QString& output) {
    _d->setDevice(output.toStdString());
}

void PlayerModel::togglePlayPause() {
    if (_d->isPlaying()) {
        _d->stop();
        _timer.stop();
    } else {
        _d->play();
        _timer.start(100);
    }
    emit isPlayingChanged();
    emit progressChanged();
}

void PlayerModel::setBegin(double value) {
    _d->setBegin(value);
}

void PlayerModel::setEnd(double value) {
    _d->setEnd(value);
}

void PlayerModel::onFiltersChanged() {
    std::vector<EqualizerNode::Filter> filters;

    QObject* p;
    foreach (p, _equalizerModel.filters()) {
        const auto f = static_cast<FilterModel*>(p);
        filters.push_back(EqualizerNode::Filter(EqualizerNode::Filter::Mode::PEAKING, f->f(), f->q(), f->g()));
    }

    _d->setFilters(filters);
}
