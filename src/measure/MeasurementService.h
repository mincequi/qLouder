#ifndef MEASUREMENTSERVICE_H
#define MEASUREMENTSERVICE_H

#include <limits>

#include <QObject>
#include <QAudioInput>
#include <QAudioOutput>

#include "AudioBuffer.h"
#include "ExcitationSignal.h"
#include "InverseFilter.h"

class Measurement;

class MeasurementService : public QObject {
	Q_OBJECT
public:
    static MeasurementService& instance();

	void setInputDevice(const QAudioDeviceInfo& audioDevice, int sampleRate);
	void setOutputDevice(const QAudioDeviceInfo& audioDevice, int sampleRate);

    void start(Signal::Channels channels, int durationPerOctaveMs, int level, double fMin, double fMax);
    void stop(bool clearResult = true);

    double progress() const;
    float level();
    float levelMax();

    QString errorString() const;
    QString errorDescription() const;

    const std::vector<float>& signal() const;

signals:
    void progressChanged(double progress);
    void measurementAvailable(Measurement* measurement);
    void levelChanged();
    void errorOccured();

private:
    explicit MeasurementService(QObject* parent = nullptr);

    void onOutputBufferEmpty();
	void onInputStateChanged(QAudio::State state);
	void onOutputStateChanged(QAudio::State state);

    double m_progress = 0.0f;
    float m_inputLevel = -99.0f;
    float m_inputLevelMax = -99.0f;

    QString m_errorString;
    QString m_errorDescription;

	AudioBuffer m_outputBuffer;
	AudioBuffer m_inputBuffer;

    ExcitationSignal _excitationSignal;
    InverseFilter _inverseFilter;

	QAudioInput* m_input = nullptr;
	QAudioOutput* m_output = nullptr;
};

#endif // MEASUREMENTSERVICE_H
