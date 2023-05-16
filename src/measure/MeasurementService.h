#pragma once

#include <QAudioInput>
#include <QAudioOutput>

#include "AudioBuffer.h"
#include "Farina.h"

class Measurement;
class ProjectManager;

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
    void levelChanged();
    void errorOccured();

private:
    explicit MeasurementService(ProjectManager& projectManager, QObject* parent = nullptr);

    void onOutputBufferEmpty();
	void onInputStateChanged(QAudio::State state);
	void onOutputStateChanged(QAudio::State state);

    ProjectManager& _projectManager;

    double m_progress = 0.0f;
    float m_inputLevel = -99.0f;
    float m_inputLevelMax = -99.0f;

    QString m_errorString;
    QString m_errorDescription;

	AudioBuffer m_outputBuffer;
	AudioBuffer m_inputBuffer;

    Farina _farina;

	QAudioInput* m_input = nullptr;
	QAudioOutput* m_output = nullptr;
};
