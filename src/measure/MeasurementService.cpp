#include "MeasurementService.h"

#include <cmath>

#include <QDebug>

#include "Config.h"
#include "FrequencyTable.h"
#include "Measurement.h"
#include "MeasurementError.h"
#include "SignalGenerator.h"
#include <status/StatusModel.h>

static QAudioFormat s_inputFormat;
static QAudioFormat s_outputFormat;

MeasurementService& MeasurementService::instance() {
    static MeasurementService instance;
    return instance;
}

MeasurementService::MeasurementService(QObject *parent)
    : QObject{parent},
    m_inputBuffer(QIODevice::WriteOnly),
    m_outputBuffer(QIODevice::ReadOnly) {
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
}

void MeasurementService::start(int durationPerOctaveMs, int volume, float fMin, float fMax) {
    if (m_durationPerOctaveMs != durationPerOctaveMs ||
            m_outputLevel != volume ||
            m_fMin != fMin ||
            m_fMax != fMax) {
        m_durationPerOctaveMs = durationPerOctaveMs;
        m_outputLevel = volume;
        m_fMin = fMin;
        m_fMax = fMax;
        m_signalLength = log2(fMax/fMin) * durationPerOctaveMs / 1000.0f * s_outputFormat.sampleRate();
        m_inputBuffer.data.clear();
        m_outputBuffer.data.clear();
        qlStatus->showMessage("Generating excitation signal");
        generateSignal();
    }

    qlStatus->showMessage("Measuring", (m_signalLength / s_outputFormat.sampleRate() + 1)*1000);
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
        auto measurement = new Measurement<float>(s_inputFormat.sampleRate(),
                                                  m_fMin,
                                                  m_fMax,
                                                  m_inputBuffer.data,
                                                  m_reverseFilter,
                                                  cfg->calibration0,
                                                  cfg->calibration90,
                                                  cfg->calibration);
        emit measurementAvailable(measurement);
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

const std::vector<float>& MeasurementService::result() const {
    return m_reverseFilter;
}

void MeasurementService::generateSignal() {
    m_outputBuffer.data.clear();
    m_outputBuffer.data.resize((m_signalLength+s_outputFormat.sampleRate()) *2);

    // "Optimizing the exponential sine sweep (ESS) signal for in situ measurements on noise barriers"
    // https://www.conforg.fr/euronoise2015/proceedings/data/articles/000420.pdf
    // Fade-In: 1 octave, Fade-Out: 1/24th octave
    const auto minIndex = FrequencyTable<double>::octaveBandsIndex(m_fMin);
    const auto maxIndex = FrequencyTable<double>::octaveBandsIndex(m_fMax);

    SignalGenerator::sineSweep(m_outputBuffer.data.begin()+s_outputFormat.sampleRate(),
                               m_outputBuffer.data.end()-s_outputFormat.sampleRate(),
                               SignalGenerator::Channels::Left,
                               s_outputFormat.sampleRate(),
                               FrequencyTable<double>::octaveBands().at(minIndex),
                               FrequencyTable<double>::octaveBands().at(maxIndex));

    SignalGenerator::fadeIn(m_outputBuffer.data.begin()+s_outputFormat.sampleRate(),
                            SignalGenerator::Channels::Left,
                            SignalGenerator::WindowFunction::Hann,
                            m_durationPerOctaveMs * s_outputFormat.sampleRate() / 1000);

    SignalGenerator::fadeOut(m_outputBuffer.data.end()-s_outputFormat.sampleRate(),
                            SignalGenerator::Channels::Left,
                            SignalGenerator::WindowFunction::Hann,
                            m_durationPerOctaveMs * s_outputFormat.sampleRate() / 24000);

    const auto outSize = m_outputBuffer.data.size()/2;
    m_reverseFilter.resize(outSize);
    for (size_t i = 0; i < outSize; ++i) {
        m_reverseFilter[outSize-1-i] = m_outputBuffer.data[i*2];
    }
    SignalGenerator::volumeEnvelope(m_reverseFilter.begin(), m_reverseFilter.end(), 20.0, 20000.0);

    SignalGenerator::volume(m_outputBuffer.data,
                            SignalGenerator::Channels::Left,
                            m_outputLevel);
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
