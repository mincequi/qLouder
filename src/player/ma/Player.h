#pragma once

#include <miniaudio.h>

#include "Engine.h"
#include "Equalizer.h"
#include "FilterNode.h"
#include "Sound.h"
#include "../PlayerInterface.h"

namespace ma {

class Player : public PlayerInterface {
public:
    Player();
    ~Player();

    const std::vector<std::string>& outputDevices() const;
    const std::vector<std::string>& inputDevices() const;

    const std::vector<std::string>& supportedFileExtensions() const override;

    bool setFile(const std::string& file) override;
    void setOutputDevice(const std::string& output) override;

    bool isPlaying() const override;
    void start() override;
    void stop() override;

    double loopBeginTime() const override;
    void setLoopBeginTime(double value) override;

    double loopEndTime() const override;
    void setLoopEndTime(double value) override;

    double progressTime() const override;
    double totalTime() const override;
    void setProgress(double value) override;

    void setFilters(const std::vector<AudioFilter>& filters) override;

    void setEqualizerEnabled(bool enable) override;
    bool isEqualizerEnabled() override;

private:
    ma_context _context;

    ma::Engine _engine;
    std::unique_ptr<ma::Sound> _sound;
    ma::Equalizer _equalizer;

    std::vector<std::string> _outputDevices;
    std::vector<std::string> _inputDevices;
    std::vector<std::string> _supportedFileExtensions = { "wav", "flac", "mp3" };

    std::string _file;
    bool _isEqualizerEnabled = true;
    //float _sampleRate = 0.0;
    //float _totalLength = 0.0;

    //Equalizer _equalizer;
};

} // namespace ma
