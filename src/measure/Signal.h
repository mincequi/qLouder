#ifndef SIGNAL_H
#define SIGNAL_H

#include <vector>

class AudioBuffer;

class Signal {
public:
    enum Channels {
        Left = 0,
        Right = 1,
        Stereo = 2
    };

    Channels channels() const;
    void setChannels(Channels channels);

    int sampleRate() const;
    void resample(int sampleRate);

    const std::vector<float>& data() const;

protected:
    Signal(AudioBuffer* buffer);

    AudioBuffer* _buffer = nullptr;
    Channels _channels = Channels::Left;
    int _sampleRate = 0;
};

#endif // SIGNAL_H
