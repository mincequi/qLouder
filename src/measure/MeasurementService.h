#ifndef MEASUREMENTSERVICE_H
#define MEASUREMENTSERVICE_H

#include <limits>

#include <QObject>
#include <QAudioInput>
#include <QAudioOutput>

#include "AudioBuffer.h"

template <class T>
class Measurement;

class MeasurementService : public QObject {
	Q_OBJECT
public:
    static MeasurementService& instance();

	void setInputDevice(const QAudioDeviceInfo& audioDevice, int sampleRate);
	void setOutputDevice(const QAudioDeviceInfo& audioDevice, int sampleRate);

    void start(int durationPerOctaveMs, int level, float fMin, float fMax);
    void stop(bool clearResult = true);

    double progress() const;
    float level();
    float levelMax();

    QString errorString() const;
    QString errorDescription() const;

    const std::vector<float>& signal() const;
	const std::vector<float>& result() const;

signals:
    void progressChanged(double progress);
    void measurementAvailable(Measurement<float>* measurement);
    void levelChanged();
    void errorOccured();

private:
    explicit MeasurementService(QObject* parent = nullptr);

	void generateSignal();

    void onOutputBufferEmpty();
	void onInputStateChanged(QAudio::State state);
	void onOutputStateChanged(QAudio::State state);

    int m_durationPerOctaveMs = 0;
    int m_signalLength = 0;
    int m_outputLevel = 0;
    double m_fMin = 0.0f;
    double m_fMax = 0.0f;

    double m_progress = 0.0f;
    float m_inputLevel = -99.0f;
    float m_inputLevelMax = -99.0f;

    QString m_errorString;
    QString m_errorDescription;

	AudioBuffer m_outputBuffer;
	AudioBuffer m_inputBuffer;
    std::vector<float> m_reverseFilter;

	QAudioInput* m_input = nullptr;
	QAudioOutput* m_output = nullptr;
};

#endif // MEASUREMENTSERVICE_H
