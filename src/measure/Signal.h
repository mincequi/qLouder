#pragma once

#include <vector>

class AudioBuffer;

class Signal {
public:
    enum Channels {
        Left = 0,
        Right = 1,
        Stereo = 2
    };

    Signal(AudioBuffer* buffer);

    Channels channels() const;
    void setChannels(Channels channels);

    int sampleRate() const;
    void resample(int sampleRate);

    const std::vector<float>& data() const;

protected:
    AudioBuffer* _buffer = nullptr;
    Channels _channels = Channels::Left;
    int _sampleRate = 0;
};
