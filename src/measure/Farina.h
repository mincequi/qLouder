#pragma once

#include "ExcitationSignal.h"
#include "ImpulseResponse.h"
#include "InverseSignal.h"

class Farina {
public:
    Farina(AudioBuffer* outputBuffer);

    const ExcitationSignal& excitationSignal();
    const ImpulseResponse& impulseResponse(const MonoSignal& measuredSignal);

    void setChannels(Signal::Channels channels);
    void setSampleRate(int rate);
    void setBeginFrequency(double f);
    void setEndFrequency(double f);
    void setDurationPerOctave(int ms);
    void setLevel(int dB);

private:
    static MonoSignal computeIr(const MonoSignal& measuredSignal, const MonoSignal& inverseSignal);

    AudioBuffer* _outputBuffer;
    ExcitationSignal _excitationSignal;
    InverseSignal _inverseSignal;
    ImpulseResponse _ir;

    bool _isDirty = true;

    Signal::Channels _channels = Signal::Channels::Left;
    int _sampleRate = 44100;
    double _beginFrequency = 20.0;
    double _endFrequency = 20000.0;
    int _durationPerOctave = 44100;
    int _level = -6;
};
