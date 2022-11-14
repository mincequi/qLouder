#include "Player.h"

#include <iostream>

#include "FilterConfig.h"

namespace ma {

Player::Player() :
    _equalizer(_engine) {
    // Init miniaudio
    ma_backend backends[] = {
        ma_backend_coreaudio,
        ma_backend_alsa,
        ma_backend_pulseaudio
    };
    ma_result result = ma_context_init(backends, 3, NULL, &_context);
    if (result != MA_SUCCESS) {
        // Error
        return;
    }

    // Populate available devices
    ma_device_info* pPlaybackDeviceInfos;
    ma_uint32 playbackDeviceCount;
    ma_device_info* pCaptureDeviceInfos;
    ma_uint32 captureDeviceCount;
    result = ma_context_get_devices(&_context,
                                    &pPlaybackDeviceInfos, &playbackDeviceCount,
                                    &pCaptureDeviceInfos, &captureDeviceCount);
    if (result != MA_SUCCESS) {
        // Error
        return;
    }
    for (uint i = 0; i < playbackDeviceCount; ++i) {
        _outputDevices.push_back(pPlaybackDeviceInfos[i].name);
    }
    for (uint i = 0; i < captureDeviceCount; ++i) {
        _inputDevices.push_back(pCaptureDeviceInfos[i].name);
    }

    _sound = std::make_unique<ma::Sound>(_engine, "");
}

Player::~Player() {
}

const std::vector<std::string>& Player::outputDevices() const {
    return _outputDevices;
}

const std::vector<std::string>& Player::inputDevices() const {
    return _inputDevices;
}

const std::vector<std::string>& Player::supportedFileExtensions() const {
    return _supportedFileExtensions;
};

bool Player::setFile(const std::string& file) {
    _file = file;

    _sound = std::make_unique<ma::Sound>(_engine, file);
    _sound->setLooping(true);

    // Create MA++ LPF
    /*
    const auto sampleRate = ma_engine_get_sample_rate(&_engine);
    FilterConfig filterConfig(FilterConfig::Type::LowPass, 1000, 0.707, 0.0, static_cast<StandardSampleRate>(sampleRate));
    _equalizer.setFilters({filterConfig});
    */

    _sound->attach(_equalizer);
    _equalizer.attach(_engine); // This should work

    return true;
};

void Player::setOutputDevice(const std::string& output) {
    //ma_engine_set_d
};

bool Player::isPlaying() const {
    return _sound->isPlaying();
    //return ma_sound_is_playing(&_sound);
};

void Player::start() {
    _sound->start();
    //ma_sound_start(&_sound);
};

void Player::stop() {
    _sound->stop();
    //ma_sound_stop(&_sound);
};

double Player::loopBeginTime() const {
    return _sound->loopBegin();
    //return 0;
};

void Player::setLoopBeginTime(double value) {
    _sound->setLoopBegin(value);
};

double Player::loopEndTime() const {
    return _sound->loopEnd();
    //return 200;
};

void Player::setLoopEndTime(double value) {
    _sound->setLoopEnd(value);
};

double Player::progressTime() const {
    return _sound->cursor();
    //return 100;
};

double Player::totalTime() const {
    //return _totalLength;
    return _sound->length();
};

void Player::setProgress(double value) {
    _sound->seekTo(value);
};

ma::FilterConfig::Type toMa(FilterType type) {
    switch (type) {
    case FilterType::Peak:      return ma::FilterConfig::Type::Peak;
    case FilterType::LowPass:   return ma::FilterConfig::Type::LowPass;
    case FilterType::HighPass:  return ma::FilterConfig::Type::HighPass;
    case FilterType::LowShelf:  return ma::FilterConfig::Type::LowShelf;
    case FilterType::HighShelf: return ma::FilterConfig::Type::HighShelf;
    default: return ma::FilterConfig::Type::None;
    }

    return ma::FilterConfig::Type::None;
}

void Player::setFilters(const std::vector<AudioFilter>& filters) {
    std::vector<FilterConfig> filters_;
    for (const auto& f : filters) {
        filters_.push_back(FilterConfig(toMa(f._type), f._f, f._q, f._g,
                                        static_cast<ma::StandardSampleRate>(_engine.sampleRate)));
    }

    _equalizer.setFilters(filters_);
};

void Player::setEqualizerEnabled(bool enable) {
    _isEqualizerEnabled = enable;

    if (!_sound)
        return;

    if (_isEqualizerEnabled) {
        _sound->attach(_equalizer);
        _equalizer.attach(_engine);
    } else {
        _sound->attach(_engine);
    }
};

bool Player::isEqualizerEnabled() {
    return _isEqualizerEnabled;
};

} // namespace ma
