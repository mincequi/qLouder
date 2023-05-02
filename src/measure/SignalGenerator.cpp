#include "SignalGenerator.h"

#include <iterator>
#include <math.h>

#include "AudioBuffer.h"

SignalFactory::SignalFactory(QObject *parent)
    : QObject{parent} {
}

void SignalFactory::sineSweep(std::vector<float>::iterator begin,
                                std::vector<float>::iterator end,
                                Signal::Channels channels,
                                int sampleRate,
                                double fMin,
                                double fMax) {
    // https://www.researchgate.net/publication/2456363_Simultaneous_Measurement_of_Impulse_Response_and_Distortion_With_a_Swept-Sine_Technique
    const int signalLength = std::distance(begin, end) / 2;
    const double T = (double)signalLength/sampleRate;
    const double fRangeLn = log(fMax/fMin);
    const double K = 2.0 * M_PI * fMin * T / fRangeLn;

    auto it = begin;
    for (int i = 0; i < signalLength; ++i) {
        *it++ = sin(K * (exp(i/(signalLength / fRangeLn)) - 1.0));
        *it++ = 0.0;
    }
}

ExcitationSignal SignalFactory::createSineSweep(AudioBuffer* buffer,
                                                Signal::Channels channels,
                                                int sampleRate,
                                                double fMin,
                                                double fMax,
                                                int samplesPerOctave,
                                                int samplesOffsetFront,
                                                int samplesOffsetBack) {
    const int signalLength = log2(fMax/fMin) * samplesPerOctave;
    const double T = (double)signalLength/sampleRate;
    const double fRangeLn = log(fMax/fMin);
    const double K = 2.0 * M_PI * fMin * T / fRangeLn;

    ExcitationSignal signal(buffer);
    signal._sampleRate = sampleRate;
    signal._minF = fMin;
    signal._maxF = fMax;
    signal._samplesPerOctave = samplesPerOctave;
    signal._samplesOffsetFront = samplesOffsetFront;
    signal._samplesOffsetBack = samplesOffsetBack;
    signal._buffer->data.clear();
    signal._buffer->data.resize(2 * (signalLength + samplesOffsetFront + samplesOffsetBack));

    auto it = signal._buffer->data.begin() + samplesOffsetFront * 2;
    for (int i = 0; i < signalLength; ++i) {
        double v = sin(K * (exp(i/(signalLength / fRangeLn)) - 1.0));
        switch (channels) {
        case Signal::Channels::Left:
            *it++ = v;
            *it++ = 0.0;
            break;
        case Signal::Channels::Right:
            *it++ = 0.0;
            *it++ = v;
            break;
        case Signal::Channels::Stereo:
            *it++ = v;
            *it++ = v;
            break;
        }
    }

    return signal;
}

/*
void SignalGenerator::sineSweep(std::vector<float>& buffer,
                                int durationPerOctaveMs,
                                int sampleRate,
                                double fMin,
                                double fMax) {
    // https://www.researchgate.net/publication/2456363_Simultaneous_Measurement_of_Impulse_Response_and_Distortion_With_a_Swept-Sine_Technique
    const double T = (log2(fMax/fMin) + 0.25f) * durationPerOctaveMs / 1000.0f;
    const int signalLength = T * sampleRate;
    const double fRangeLn = log(fMax/fMin);
    const double K = 2.0 * M_PI * fMin * T / fRangeLn;

    buffer.resize(signalLength);
    for (int i = 0; i < signalLength; ++i) {
        buffer[i] = sin(K * (exp(i/(signalLength / fRangeLn)) - 1.0));
    }
}
*/

void SignalFactory::window(std::vector<float>::iterator begin,
                             std::vector<float>::iterator end,
                             Signal::Channels channels,
                             WindowFunction function,
                             int K) {
    auto it = begin;
    const double factor = 2.0 * M_PI / (2*K + 1);
    for (int i = 0; i < K; ++i) {
        *it++ *= 0.5 * (1.0 - cos(factor * i));
        ++it;
    }

    it = end - 2*K;
    int j = 0;
    for (int i = K + 1; i < 2*K + 1; ++i, ++j) {
        *it++ *= 0.5 * (1.0 - cos(factor * i));
        ++it;
    }
}

void SignalFactory::fadeIn(std::vector<float>::iterator begin,
                             Signal::Channels channels,
                             WindowFunction,
                             int K) {
    auto it = begin;
    const double factor = 2.0 * M_PI / (2*K + 1);
    for (int i = 0; i <= K; ++i) {
        *it++ *= 0.5 * (1.0 - cos(factor * i));
        ++it;
    }
}

void SignalFactory::fadeOut(std::vector<float>::iterator end,
                              Signal::Channels channels,
                              WindowFunction,
                              int K) {
    auto it = end - 2*K;
    const double factor = 2.0 * M_PI / (2*K + 1);
    int j = 0;
    for (int i = K + 1; i < 2*K + 1; ++i, ++j) {
        *it++ *= 0.5 * (1.0 - cos(factor * i));
        ++it;
    }
}

void SignalFactory::volume(std::vector<float>& buffer,
                             Signal::Channels channels,
                             int levelDb) {
    // Apply volume
    if (levelDb != 0) {
        const double factor = pow(10.0, levelDb/20.0);
        const int signalLength = buffer.size()/2;
        for (int i = 0; i < signalLength; ++i) {
            buffer[i*2] *= factor;
        }
    }
}

void SignalFactory::volumeEnvelope(std::vector<float>::iterator begin,
                                     std::vector<float>::iterator end,
                                     double fMin,
                                     double fMax) {
    const int signalLength = std::distance(begin, end);
    const double factor = log2(fMax/fMin)/signalLength;

    auto it = begin;
    for (int i = 0; i < signalLength; ++i) {
        *it++ *= pow(0.5, factor * i);
    }
}
