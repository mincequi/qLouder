#pragma once

#include <string>
#include <vector>

#include "AudioFilter.h"

class PlayerInterface {
public:
    virtual ~PlayerInterface() {}

    virtual const std::vector<std::string>& supportedFileExtensions() const = 0;

    virtual bool setFile(const std::string& file) = 0;
    virtual void setOutputDevice(const std::string& output) = 0;

    virtual bool isPlaying() const = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

    virtual double loopBeginTime() const = 0;
    virtual void setLoopBeginTime(double value) = 0;

    virtual double loopEndTime() const = 0;
    virtual void setLoopEndTime(double value) = 0;

    virtual double progressTime() const = 0;
    virtual double totalTime() const = 0;
    virtual void setProgress(double value) = 0;

    virtual void setFilters(const std::vector<AudioFilter>& filters) {};

    virtual void setEqualizerEnabled(bool enable) = 0;
    virtual bool isEqualizerEnabled() = 0;
};
