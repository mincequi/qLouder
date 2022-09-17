#include "PlayerModel.h"

#include <iostream>

#include <QSettings>

#include <taglib/fileref.h>

#include <equalizer/EqualizerModel.h>
#include <equalizer/FilterModel.h>
#include <measure/MeasureModel.h>
#include <player/EqualizerNode.h>

#include "PlayerModelPrivate.h"

static const QString sampleKey("sample");

using namespace cinder::audio;

PlayerModel::PlayerModel(const EqualizerModel& equalizerModel,
                         QObject *parent)
    : QObject{parent},
      _equalizerModel(equalizerModel),
      _d(new PlayerModelPrivate) {
    connect(&_timer, &QTimer::timeout, this, &PlayerModel::progressChanged);
    connect(&_equalizerModel, &EqualizerModel::rangeChanged, this, &PlayerModel::onFiltersChanged);

    QSettings settings;
    auto sample = settings.value(sampleKey).toString();
    if (!sample.isEmpty()) {
        setFile(sample);
    }
}

PlayerModel::~PlayerModel() {
    QSettings settings;
    settings.setValue(sampleKey, _file);

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

double PlayerModel::loopBeginTime() const {
    return _d->loopBeginTime();
}

double PlayerModel::loopEndTime() const {
    return _d->loopEndTime();
}

double PlayerModel::totalTime() const {
    return _d->totalTime();
}

double PlayerModel::progress() const {
    return (_d->progressTime() - _d->loopBeginTime()) /
            (_d->loopEndTime() - _d->loopBeginTime());
}

QString PlayerModel::title() const {
    return _title;
}

void PlayerModel::setFile(const QString& file) {
    // Create title from tags. Print file name if something goes wrong.
    TagLib::FileRef f(file.toStdString().c_str(), false);
    if (f.tag() == nullptr || f.tag()->artist().isEmpty() || f.tag()->title().isEmpty()) {
        _title = file.mid(file.lastIndexOf("/")+1);
    } else {
        _title = QString::fromWCharArray(f.tag()->artist().toCWString()) + " - "
                 + QString::fromWCharArray(f.tag()->title().toCWString());
    }

    if (_d->setFile(file.toStdString())) {
        _file = file;
        emit fileChanged();
        emit statusChanged();
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
    emit statusChanged();
    emit progressChanged();
}

void PlayerModel::setBegin(double value) {
    _d->setLoopBeginTime(value);
    emit statusChanged();
}

void PlayerModel::setEnd(double value) {
    _d->setLoopEndTime(value);
    emit statusChanged();
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
