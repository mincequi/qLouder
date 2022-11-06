#include "PulseEffectsExporter.h"

#include <algorithm>

#include <QDebug>
#include <QGSettings/QGSettings>

#include "equalizer/EqualizerModel.h"
#include "equalizer/FilterModel.h"

namespace tag {
inline constexpr auto id = "com.github.wwmm.pulseeffects.sinkinputs.equalizer";
inline constexpr auto id_left = "com.github.wwmm.pulseeffects.sinkinputs.equalizer.leftchannel";
inline constexpr auto id_right = "com.github.wwmm.pulseeffects.sinkinputs.equalizer.rightchannel";
inline constexpr auto path = "/com/github/wwmm/pulseeffects/sinkinputs/equalizer/";
inline constexpr auto path_left = "/com/github/wwmm/pulseeffects/sinkinputs/equalizer/leftchannel/";
inline constexpr auto path_right= "/com/github/wwmm/pulseeffects/sinkinputs/equalizer/rightchannel/";
}

std::unique_ptr<PulseEffectsExporter> PulseEffectsExporter::create(const EqualizerModel& eq) {
    if (!QGSettings::isSchemaInstalled(tag::id)) {
        return nullptr;
    }
    if (!QGSettings::isSchemaInstalled(tag::id_left)) {
        return nullptr;
    }
    if (!QGSettings::isSchemaInstalled(tag::id_right)) {
        return nullptr;
    }

    // Due to private constructor make_unique does not work here
    return std::unique_ptr<PulseEffectsExporter>(new PulseEffectsExporter(eq));
    //return std::make_unique<PulseEffectsExporter>();
}

void PulseEffectsExporter::save() {
    doExportGeneral();
    doExportFilters();
}

void PulseEffectsExporter::bind(bool bind_) {
    if (_isBound == bind_) {
        return;
    }
    _isBound = bind_;

    if (_isBound) {
        doExportGeneral();
        doExportFilters();
        connect(&_eq, &EqualizerModel::rangeChanged, this, &PulseEffectsExporter::doExportFilters);
    } else {
        disconnect(&_eq, &EqualizerModel::rangeChanged, this, &PulseEffectsExporter::doExportFilters);
    }
}

bool PulseEffectsExporter::isBound() const {
    return _isBound;
}

PulseEffectsExporter::PulseEffectsExporter(const EqualizerModel& eq) :
    _eq(eq) {
    _settings = new QGSettings(tag::id, tag::path, this);
    _leftSettings = new QGSettings(tag::id_left, tag::path_left, this);
    _rightSettings = new QGSettings(tag::id_right, tag::path_right, this);

    connect(_settings, &QGSettings::changed, this, &PulseEffectsExporter::onValueChanged);
    // This seg faults whenever a value is changed complaining about key not find in regalur schema (not the channel schema)
    //connect(_leftSettings, &QGSettings::changed, this, &PulseEffectsExporter::onLeftValueChanged);
    //connect(_rightSettings, &QGSettings::changed, this, &PulseEffectsExporter::onRightValueChanged);
}

void PulseEffectsExporter::onValueChanged(const QString& key) {
    qDebug() << "onValueChanged>" << key << ":" << _settings->get(key);
}

void PulseEffectsExporter::onLeftValueChanged(const QString& key) {
    qDebug() << "onLeftValueChanged>" << key << ":" << _settings->get(key);
}

void PulseEffectsExporter::onRightValueChanged(const QString& key) {
    qDebug() << "onRightValueChanged>" << key << ":" << _settings->get(key);
}

void PulseEffectsExporter::doExportGeneral() {
    _settings->set("state", _isBound);
    _settings->set("postMessages", _isBound);
    if (_isBound) {
        _settings->set("mode", "IIR");
        _settings->set("splitChannels", false);
        for (int i = 0; i < 30; ++i) {
            _leftSettings->set("band" + QString::number(i) + "Mode", "RLC (BT)");
            _leftSettings->set("band" + QString::number(i) + "Mute", false);
            _leftSettings->set("band" + QString::number(i) + "Slope", "x1");
            _leftSettings->set("band" + QString::number(i) + "Solo", false);
            _rightSettings->set("band" + QString::number(i) + "Mode", "RLC (BT)");
            _rightSettings->set("band" + QString::number(i) + "Mute", false);
            _rightSettings->set("band" + QString::number(i) + "Slope", "x1");
            _rightSettings->set("band" + QString::number(i) + "Solo", false);
        }
    }
}

QString toEe(int type) {
    auto type_ = static_cast<FilterType>(type);
    switch (type_) {
    case FilterType::Peak:
        return "Bell";
    case FilterType::LowPass:
        return "Lo-pass";
    case FilterType::HighPass:
        return "Hi-pass";
    case FilterType::LowShelf:
        return "Lo-shelf";
    case FilterType::HighShelf:
        return "Hi-shelf";
    default:
        return "Off";
    }
    return "Off";
}

void PulseEffectsExporter::doExportFilters() {
    _settings->set("numBands", std::clamp(_eq.filterCount(), 1, 30));
    for (int i = 0; i < _eq.filterCount(); ++i) {
        _leftSettings->set("band" + QString::number(i) + "Type", toEe(_eq.filter(i)->type()));
        _leftSettings->set("band" + QString::number(i) + "Frequency", _eq.filter(i)->f());
        _leftSettings->set("band" + QString::number(i) + "Gain", _eq.filter(i)->g());
        _leftSettings->set("band" + QString::number(i) + "Q", _eq.filter(i)->q());
        _rightSettings->set("band" + QString::number(i) + "Type", toEe(_eq.filter(i)->type()));
        _rightSettings->set("band" + QString::number(i) + "Frequency", _eq.filter(i)->f());
        _rightSettings->set("band" + QString::number(i) + "Gain", _eq.filter(i)->g());
        _rightSettings->set("band" + QString::number(i) + "Q", _eq.filter(i)->q());

    }
    for (int i = _eq.filterCount(); i < 30; ++i) {
        _leftSettings->set("band" + QString::number(i) + "Type", "Off");
        _rightSettings->set("band" + QString::number(i) + "Type", "Off");
    }
}
