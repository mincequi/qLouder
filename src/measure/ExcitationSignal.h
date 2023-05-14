#pragma once

#include "Signal.h"

class AudioBuffer;

/**
 * @brief The ExcitationSignal class
 *
 * Stereo only
 */
class ExcitationSignal : public Signal {
public:
    ExcitationSignal(AudioBuffer* buffer);

    int volume() const;
    void setVolume(int db);

    double minF() const;
    double maxF() const;
    int samplesPerOctave() const;
    int samplesOffsetFront() const;
    int samplesOffsetBack() const;

    /**
     * @brief fadeIn
     * @param K the number of samples to fade
     */
    void fadeIn(int K);

    /**
     * @brief fadeOut
     * @param K the number of samples to fade
     */
    void fadeOut(int K);

private:
    double _minF = 20.0;
    double _maxF = 20000.0;
    int _samplesPerOctave = 0;
    int _samplesOffsetFront = 0;
    int _samplesOffsetBack = 0;
    int _volume = 0;

    friend class SignalFactory;
};
