#include "PlayerModel.h"

#include <QDebug>
#include <QSettings>

#include <taglib/fileref.h>

#include <equalizer/EqualizerModel.h>
#include <equalizer/FilterModel.h>
#include <measure/MeasureModel.h>
#include <ma/FilterConfig.h>
#include <ma/Player.h>

static const QString sampleKey("sample");

PlayerModel::PlayerModel(const EqualizerModel& equalizerModel,
                         QObject *parent)
    : QObject{parent},
      _equalizerModel(equalizerModel),
      //_d(new PlayerModelPrivate) {
      _d(new ma::Player) {
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
    const auto value = _d->loopBeginTime();
    qDebug() << "loopBeginTime:" << value;
    return value;
}

double PlayerModel::loopEndTime() const {
    const auto value = _d->loopEndTime();
    qDebug() << "loopEndTime:" << value;
    return value;
}

double PlayerModel::totalTime() const {
    return _d->totalTime();
}

double PlayerModel::progress() const {
    const auto value = (_d->progressTime() - _d->loopBeginTime()) /
                       (_d->loopEndTime() - _d->loopBeginTime());
    //const auto value = _d->progressTime();
    //qDebug() << "progress:" << value;
    return value;
}

QString PlayerModel::title() const {
    return _title;
}

bool PlayerModel::isEqualizerEnabled() const {
    return _d->isEqualizerEnabled();
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
    _d->setOutputDevice(output.toStdString());
}

void PlayerModel::togglePlayPause() {
    if (_d->isPlaying()) {
        _d->stop();
        _timer.stop();
    } else {
        _d->start();
        _timer.start(100);
    }
    emit statusChanged();
    emit progressChanged();
}

void PlayerModel::setBegin(double value) {
    _d->setLoopBeginTime(value);
    //qDebug() << "loopBegin:" << value;
    emit statusChanged();
    emit progressChanged();
}

void PlayerModel::setEnd(double value) {
    _d->setLoopEndTime(value);
    //qDebug() << "loopEnd:" << value;
    emit statusChanged();
    emit progressChanged();
}

void PlayerModel::toggleEqualizer() {
    _d->setEqualizerEnabled(!_d->isEqualizerEnabled());
    emit statusChanged();
}

void PlayerModel::setProgress(double value) {
    _d->setProgress(value);
    emit progressChanged();
    //qDebug() << "Seek to:" << value;
}

void PlayerModel::onFiltersChanged() {
    std::vector<AudioFilter> filters;
    const FilterModel* f;
    foreach (f, _equalizerModel._filters) {
        filters.push_back(AudioFilter(static_cast<FilterType>(f->type()), f->f(), f->g(), f->q()));
    }

    _d->setFilters(filters);
}
