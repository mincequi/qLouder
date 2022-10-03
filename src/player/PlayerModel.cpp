#include "PlayerModel.h"

#include <QSettings>

#include <taglib/fileref.h>
#include <cinder/audio/EqualizerNode.h>

#include <equalizer/EqualizerModel.h>
#include <equalizer/FilterModel.h>
#include <measure/MeasureModel.h>

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
    emit progressChanged();
}

void PlayerModel::setEnd(double value) {
    _d->setLoopEndTime(value);
    emit statusChanged();
    emit progressChanged();
}

void PlayerModel::toggleEqualizer() {
    _d->toggleEqualizer();
    emit statusChanged();
}

void PlayerModel::setProgress(double value) {
    _d->setProgress(value);
    emit progressChanged();
}

EqualizerNode::Filter::Mode toCinder(int type_) {
    FilterType type = (FilterType)type_;
    switch (type) {
    case FilterType::Peak: return EqualizerNode::Filter::Mode::PEAKING;
    case FilterType::LowPass: return EqualizerNode::Filter::Mode::LOWPASS;
    case FilterType::HighPass: return EqualizerNode::Filter::Mode::HIGHPASS;
    case FilterType::LowShelf: return EqualizerNode::Filter::Mode::LOWSHELF;
    case FilterType::HighShelf: return EqualizerNode::Filter::Mode::HIGHSHELF;
    case FilterType::AllPass: return EqualizerNode::Filter::Mode::ALLPASS;
    default: return EqualizerNode::Filter::Mode::ALLPASS;
    }

    return EqualizerNode::Filter::Mode::ALLPASS;
}

void PlayerModel::onFiltersChanged() {
    std::vector<EqualizerNode::Filter> filters;

    const FilterModel* f;
    foreach (f, _equalizerModel._filters) {
        if (f->type() != 0) {
            filters.push_back(EqualizerNode::Filter(toCinder(f->type()), f->f(), f->q(), f->g()));
        } else {
            // If invalid type, we pass a pass through biquad (peaking with q and gain of 0 dB)
            filters.push_back(EqualizerNode::Filter(EqualizerNode::Filter::Mode::PEAKING, -1.0, 0.0, 0.0));
        }
    }

    _d->setFilters(filters);
}
