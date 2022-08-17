#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H

#include <QObject>

#include <vector>

#include "ExcitationSignal.h"

class SignalFactory : public QObject {
    Q_OBJECT
public:
    enum WindowFunction {
        Hann
    };

    explicit SignalFactory(QObject *parent = nullptr);

    static void sineSweep(std::vector<float>::iterator begin,
                          std::vector<float>::iterator end,
                          Signal::Channels channels,
                          int sampleRate,
                          double fMin,
                          double fMax);

    static ExcitationSignal createSineSweep(AudioBuffer* buffer,
                                            Signal::Channels channels,
                                            int sampleRate,
                                            double fMin,
                                            double fMax,
                                            int samplesPerOctave,
                                            int samplesOffsetFront = 0,
                                            int samplesOffsetBack = 0);

    /*
    static void sineSweep(std::vector<float>& buffer,
                          int durationPerOctaveMs,
                          int sampleRate,
                          double fMin,
                          double fMax);
    */

    static void window(std::vector<float>::iterator begin,
                       std::vector<float>::iterator end,
                       Signal::Channels channels,
                       WindowFunction,
                       int K);

    static void fadeIn(std::vector<float>::iterator begin,
                       Signal::Channels channels,
                       WindowFunction,
                       int K);

    static void fadeOut(std::vector<float>::iterator end,
                        Signal::Channels channels,
                        WindowFunction,
                        int K);

    static void volume(std::vector<float>& buffer,
                       Signal::Channels channels,
                       int levelDb);

    static void volumeEnvelope(std::vector<float>::iterator begin,
                               std::vector<float>::iterator end,
                               double fMin,
                               double fMax);

signals:

};

#endif // SIGNALGENERATOR_H
