#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H

#include <QObject>

#include <vector>

class SignalGenerator : public QObject {
    Q_OBJECT
public:
    enum Channels {
        Left = 1,
        Right = 2,
        Both = 3
    };
    enum WindowFunction {
        Hann
    };

    explicit SignalGenerator(QObject *parent = nullptr);

    static void sineSweep(std::vector<float>::iterator begin,
                          std::vector<float>::iterator end,
                          Channels channels,
                          int sampleRate,
                          double fMin,
                          double fMax);

    static void sineSweep(std::vector<float>& buffer,
                          int durationPerOctaveMs,
                          int sampleRate,
                          double fMin,
                          double fMax);

    static void window(std::vector<float>::iterator begin,
                       std::vector<float>::iterator end,
                       Channels channels,
                       WindowFunction,
                       int K);

    static void fadeIn(std::vector<float>::iterator begin,
                       Channels channels,
                       WindowFunction,
                       int K);

    static void fadeOut(std::vector<float>::iterator end,
                        Channels channels,
                        WindowFunction,
                        int K);

    static void volume(std::vector<float>& buffer,
                       Channels channels,
                       int levelDb);

    static void volumeEnvelope(std::vector<float>::iterator begin,
                               std::vector<float>::iterator end,
                               double fMin,
                               double fMax);

signals:

};

#endif // SIGNALGENERATOR_H
