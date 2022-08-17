#include "ExcitationSignal.h"

#include <math.h>

#include "AudioBuffer.h"

ExcitationSignal::ExcitationSignal(AudioBuffer* buffer) :
    Signal(buffer) {
}

int ExcitationSignal::volume() const {
    return _volume;
}

void ExcitationSignal::setVolume(int levelDb) {
    if (levelDb != _volume) {
        const double factor = pow(10.0, levelDb/20.0);
        for (auto& s : _buffer->data) {
            s *= factor;
        }
    }
}

double ExcitationSignal::minF() const {
    return _minF;
}

double ExcitationSignal::maxF() const {
    return _maxF;
}

int ExcitationSignal::samplesPerOctave() const {
    return _samplesPerOctave;
}

int ExcitationSignal::samplesOffsetFront() const {
    return _samplesOffsetFront;
}

int ExcitationSignal::samplesOffsetBack() const {
    return _samplesOffsetBack;
}

void ExcitationSignal::fadeIn(int K) {
    auto it = _buffer->data.begin() + _samplesOffsetFront * 2;
    const double factor = 2.0 * M_PI / (2*K + 1);
    for (int i = 0; i <= K; ++i) {
        const double d = 0.5 * (1.0 - cos(factor * i));
        *it++ *= d;
        *it++ *= d;
    }
}

void ExcitationSignal::fadeOut(int K) {
    auto it = _buffer->data.end() - (_samplesOffsetBack * 2) - (2*K);
    const double factor = 2.0 * M_PI / (2*K + 1);
    for (int i = K + 1; i < 2*K + 1; ++i) {
        const double d = 0.5 * (1.0 - cos(factor * i));
        *it++ *= d;
        *it++ *= d;
    }
}
