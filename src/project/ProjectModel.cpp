#include "ProjectModel.h"

#include <algorithm>

#include <QAudioDeviceInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QUrl>

#include "Config.h"
#include "measure/MeasurementService.h"
#include "util/FrFile.h"

static const QString calibration0Key("calibration0");
static const QString calibration90Key("calibration90");
static const QString inputDeviceKey("inputDevice");
static const QString inputSampleRateKey("inputSampleRate");
static const QString outputDeviceKey("outputDevice");
static const QString outputSampleRateKey("outputSampleRate");

QStringList sampleRatesToStringList(const QList<int>& in) {
	QStringList out;

	/*
	int s = 0;
	foreach (s, in) {
		if (s % 1000) {
			out << QString::number(s/1000.0, 'f', 1) + " kHz";
		} else {
			out << QString::number(s/1000) + " kHz";
		}
	}
	*/

	int s = 0;
	foreach (s, in) {
		//out << QString::number(s) + " Hz";
		//out << QString::number(s/1000.0, 'f', 1) + " kHz";
		out << QString::number(s/1000) + " kHz";
	}

	return out;
}

int findDevice(const QList<QAudioDeviceInfo>& list, const QString& name) {
    int i = 0;
    for (const auto& d : list) {
        if (d.deviceName() == name)
            return i;
        else
            ++i;
    }

    return 0;
}

ProjectModel::ProjectModel(MeasurementService& measurementService, QObject *parent)
	: QObject{parent},
	  m_measurementService(measurementService) {
	m_projectDir = QStandardPaths::displayName(QStandardPaths::DocumentsLocation);
	m_inputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
	m_outputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);

    QSettings settings;
    auto calibration0 = settings.value(calibration0Key).toString();
    if (!calibration0.isEmpty()) {
        openCalibrationFile(0, calibration0);
    }
    auto calibration90 = settings.value(calibration90Key).toString();
    if (!calibration90.isEmpty()) {
        openCalibrationFile(90, calibration90);
    }
}

ProjectModel::~ProjectModel() {
    QSettings settings;
    if (m_micCalibration0.startsWith("Error")) {
        m_micCalibration0.clear();
    }
    settings.setValue(calibration0Key, m_micCalibration0);

    if (m_micCalibration90.startsWith("Error")) {
        m_micCalibration90.clear();
    }
    settings.setValue(calibration90Key, m_micCalibration90);

    settings.setValue(inputDeviceKey, m_inputDeviceIndex ? m_inputDevices.at(m_inputDeviceIndex).deviceName() : "<invalid>");
    settings.setValue(outputDeviceKey, m_outputDeviceIndex ? m_outputDevices.at(m_outputDeviceIndex).deviceName() : "<invalid>");
    settings.setValue(inputSampleRateKey, m_inputDeviceIndex ? m_inputSampleRateIndex : 0);
    settings.setValue(outputSampleRateKey, m_outputDeviceIndex ? m_outputSampleRateIndex : 0);
}

QStringList ProjectModel::inputDevices() {
	QStringList out;
	QAudioDeviceInfo device;
	foreach (device, m_inputDevices) {
		out << device.deviceName();
	}
	return out;
}

void ProjectModel::setInputDevice(int index) {
	m_inputDeviceIndex = index;
    //m_inputSampleRateIndex = 0;
	// TODO: only add the preferred sample rate for now. Others might cause glitches
    //m_inputSampleRates = { m_inputDevices.at(m_inputDeviceIndex).preferredFormat().sampleRate() };
    //m_inputSampleRates = m_inputDevices.at(m_inputDeviceIndex).supportedSampleRates();
    //std::sort(m_inputSampleRates.begin(), m_inputSampleRates.end());

    // Try to open input device to raise privacy popup from OS
    openInputDevice(m_inputDevices.at(m_inputDeviceIndex));

	emit inputDeviceChanged();
}

int ProjectModel::savedInputDevice() const {
    QSettings settings;
    const auto idx = findDevice(m_inputDevices, settings.value(inputDeviceKey).toString());
    return idx;
}

QStringList ProjectModel::inputSampleRates() {
	return sampleRatesToStringList(m_inputSampleRates);
}

void ProjectModel::setInputSampleRate(int index) {
	m_inputSampleRateIndex = index;
	m_measurementService.setInputDevice(m_inputDevices.at(m_inputDeviceIndex), m_inputSampleRates.at(m_inputSampleRateIndex));
}

int ProjectModel::savedInputSampleRate() const {
    QSettings settings;
    const int idx = settings.value(inputSampleRateKey, 0).toInt();
    return idx;
}

QStringList ProjectModel::outputDevices() {
	QStringList out;
	QAudioDeviceInfo device;
	foreach (device, m_outputDevices) {
		out << device.deviceName();
	}
	return out;
}

void ProjectModel::setOutputDevice(int index) {
	m_outputDeviceIndex = index;
    //m_outputSampleRateIndex = 0;
	// TODO: only add the preferred sample rate for now. Others might cause glitches
    //m_outputSampleRates = { m_outputDevices.at(m_outputDeviceIndex).preferredFormat().sampleRate() };
    //m_outputSampleRates = m_outputDevices.at(m_outputDeviceIndex).supportedSampleRates();
    //std::sort(m_outputSampleRates.begin(), m_outputSampleRates.end());
	emit outputDeviceChanged();
}

int ProjectModel::savedOutputDevice() const {
    QSettings settings;
    const auto idx = findDevice(m_outputDevices, settings.value(outputDeviceKey).toString());
    return idx;
}

QStringList ProjectModel::outputSampleRates() {
	return sampleRatesToStringList(m_outputSampleRates);
}

void ProjectModel::setOutputSampleRate(int index) {
	m_outputSampleRateIndex = index;
	m_measurementService.setOutputDevice(m_outputDevices.at(m_outputDeviceIndex), m_outputSampleRates.at(m_outputSampleRateIndex));
}

int ProjectModel::savedOutputSampleRate() const {
    QSettings settings;
    const int idx = settings.value(outputSampleRateKey, 0).toInt();
    return idx;
}

void ProjectModel::openCalibrationFile(int degrees, const QString& fileName) {
    if (degrees == 0) {
        cfg->calibration0 = FrFile<double>::read(fileName);
        m_micCalibration0 = cfg->calibration0.empty() ? "Error parsing: " : "";
        m_micCalibration0 += fileName;
    } else if (degrees == 90) {
        cfg->calibration90 = FrFile<double>::read(fileName);
        m_micCalibration90 = cfg->calibration90.empty() ? "Error parsing: " : "";
        m_micCalibration90 += fileName;
    }

    emit modelChanged();
}

void ProjectModel::openInputDevice(const QAudioDeviceInfo& device) {
    QAudioFormat inputFormat;
    inputFormat.setByteOrder(QAudioFormat::LittleEndian);
    inputFormat.setChannelCount(1);
    inputFormat.setCodec("audio/pcm");
    inputFormat.setSampleRate(m_inputSampleRates[m_inputSampleRateIndex]);
    inputFormat.setSampleSize(32);
    inputFormat.setSampleType(QAudioFormat::Float);
    QAudioInput input(device, inputFormat);
    input.start();
    input.stop();
}
