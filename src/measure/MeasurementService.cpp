#include "MeasurementService.h"

#include <cmath>

#include <QDebug>

#include "MeasurementError.h"
#include "ResponseSignal.h"
#include "SignalGenerator.h"
#include <common/Config.h>
#include <project/Project.h>
#include <project/ProjectManager.h>
#include <status/StatusModel.h>

static QAudioFormat s_inputFormat;
static QAudioFormat s_outputFormat;

MeasurementService& MeasurementService::instance() {
    static MeasurementService instance(ProjectManager::instance());
    return instance;
}

MeasurementService::MeasurementService(ProjectManager& projectManager, QObject *parent)
    : QObject{parent},
      _projectManager(projectManager),
      m_outputBuffer(QIODevice::ReadOnly),
      m_inputBuffer(QIODevice::WriteOnly),
      _farina(&m_outputBuffer) {
    s_inputFormat.setByteOrder(QAudioFormat::LittleEndian);
    s_inputFormat.setChannelCount(1);
    s_inputFormat.setCodec("audio/pcm");
    s_inputFormat.setSampleRate(44100);
    s_inputFormat.setSampleSize(32);
    s_inputFormat.setSampleType(QAudioFormat::Float);

    s_outputFormat.setByteOrder(QAudioFormat::LittleEndian);
    s_outputFormat.setChannelCount(2);
    s_outputFormat.setCodec("audio/pcm");
    s_outputFormat.setSampleRate(44100);
    s_outputFormat.setSampleSize(32);
    s_outputFormat.setSampleType(QAudioFormat::Float);

    // We need to use a queued connection, otherwise we cause a loop (or a real-time violation).
    connect(&m_outputBuffer, &AudioBuffer::ranEmpty, this, &MeasurementService::onOutputBufferEmpty, Qt::QueuedConnection);

    m_input = new QAudioInput(s_inputFormat);
    m_output = new QAudioOutput(s_outputFormat);
}

void MeasurementService::setInputDevice(const QAudioDeviceInfo& audioDevice, int sampleRate) {
    m_input->deleteLater();

    s_inputFormat.setSampleRate(sampleRate);

    m_input = new QAudioInput(audioDevice, s_inputFormat);
    m_input->setBufferSize(65536);
    m_input->setNotifyInterval(250); // 4 Hz
    connect(m_input, &QAudioInput::stateChanged, this, &MeasurementService::onInputStateChanged);
    connect(m_input, &QAudioInput::notify, this, [this] {
        const auto level = m_inputBuffer.level();
        if (level < -99.0) {
            return;
        }
        m_inputLevel = level;
        m_inputLevelMax = std::max(m_inputLevelMax, m_inputLevel);
        emit levelChanged();
    });
    qDebug() << "audio input device set to:" << audioDevice.deviceName();
}

void MeasurementService::setOutputDevice(const QAudioDeviceInfo& audioDevice, int sampleRate) {
    m_output->deleteLater();

    s_outputFormat.setSampleRate(sampleRate);

    m_output = new QAudioOutput(audioDevice, s_outputFormat);
    m_output->setBufferSize(65536);
    m_output->setNotifyInterval(25); // 40 Hz
    //qDebug() << "output buffer size: " << m_output->bufferSize() << ", period size: " << m_output->periodSize();
    connect(m_output, &QAudioOutput::stateChanged, this, &MeasurementService::onOutputStateChanged);
    connect(m_output, &QAudioOutput::notify, this, [this] {
        const auto usecs = m_output->processedUSecs();
        m_progress = (usecs/1000000.0)/(m_outputBuffer.data.size()*0.5/s_outputFormat.sampleRate());
        emit progressChanged(m_progress);
    });
    qDebug() << "audio output device set to:" << audioDevice.deviceName();
}

void MeasurementService::start(Signal::Channels channels, int durationPerOctaveMs, int volume, double fMin, double fMax) {
    _farina.setSampleRate(s_outputFormat.sampleRate());
    _farina.setChannels(channels);
    _farina.setDurationPerOctave(durationPerOctaveMs);
    _farina.setLevel(volume);
    _farina.setBeginFrequency(fMin);
    _farina.setEndFrequency(fMax);

    _farina.excitationSignal();

    auto signalLength = log2(fMax/fMin) * durationPerOctaveMs;
    qlStatus->showMessage("Measuring", signalLength + 1);
    qDebug() << "output buffer size: " << m_output->bufferSize() << ", period size: " << m_output->periodSize();
    m_progress = 0.0f;
    m_inputLevel = -99.0f;
    m_inputLevelMax = -99.0f;
    m_outputBuffer.start();
    m_inputBuffer.start();
    m_output->start(&m_outputBuffer);
    m_input->start(&m_inputBuffer);
    if (m_output->error() != QAudio::NoError || m_input->error() != QAudio::NoError) {
        qDebug() << "output error: " << m_output->error() << ", input error: " << m_input->error();
        MeasurementError::errorToString(m_output->error(), m_errorString, m_errorDescription);
        if (!m_errorString.isEmpty()) {
            emit errorOccured();
        }
        stop();
    }
}

void MeasurementService::stop(bool clearResult) {
    m_input->stop();
    m_output->stop();
    m_inputBuffer.stop();
    m_outputBuffer.stop();
    if (clearResult) {
        qlStatus->showMessage("Measuring failed", 3000);
        m_inputBuffer.data.resize(0);
    } else {
        qlStatus->clearMessage();

        auto responseSignal = ResponseSignal(&m_inputBuffer, s_inputFormat.sampleRate());
        responseSignal.resample(s_outputFormat.sampleRate());

        auto measurement = new Project(s_outputFormat.sampleRate(),
                                           _farina.impulseResponse(responseSignal.data()),
                                           cfg->calibration0,
                                           cfg->calibration90,
                                           cfg->calibration);
        _projectManager.add(measurement);
    }
    m_progress = 0.0f;
    m_inputLevel = -99.0f;
    m_inputLevelMax = -99.0f;
    emit levelChanged();
    emit progressChanged(m_progress);
}

double MeasurementService::progress() const {
    return m_progress;
}

float MeasurementService::level() {
    return m_inputLevel;
}

float MeasurementService::levelMax() {
    return m_inputLevelMax;
}

QString MeasurementService::errorString() const {
    return m_errorString;
}

QString MeasurementService::errorDescription() const {
    return m_errorDescription;
}

const std::vector<float>& MeasurementService::signal() const {
    return m_outputBuffer.data;
}

void MeasurementService::onOutputBufferEmpty() {
    stop(false);
}

void MeasurementService::onInputStateChanged(QAudio::State state) {
    qDebug() << "input state: " << state;

    if (m_input->error() != QAudio::NoError) {
        qDebug() << "input error: " << m_input->error();
        MeasurementError::errorToString(m_input->error(), m_errorString, m_errorDescription);
        if (!m_errorString.isEmpty()) {
            emit errorOccured();
        }
        stop();
    }
}

void MeasurementService::onOutputStateChanged(QAudio::State state) {
    qDebug() << "output state: " << state;

    if (m_output->error() != QAudio::NoError) {
        qDebug() << "output error: " << m_output->error();
        MeasurementError::errorToString(m_output->error(), m_errorString, m_errorDescription);
        if (!m_errorString.isEmpty()) {
            emit errorOccured();
        }
        stop();
    }
}
