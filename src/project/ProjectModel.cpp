#include "ProjectModel.h"

#include <algorithm>

#include <QAudioDeviceInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QUrl>

#include "Config.h"
#include "measure/MeasurementService.h"
#include "util/FrFile.h"

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

ProjectModel::ProjectModel(MeasurementService& measurementService, QObject *parent)
	: QObject{parent},
	  m_measurementService(measurementService) {
	m_projectDir = QStandardPaths::displayName(QStandardPaths::DocumentsLocation);
	m_inputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
	m_outputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);

    QSettings settings;
    auto calibration0 = settings.value("calibration0").toString();
    if (!calibration0.isEmpty()) {
        openCalibrationFile(0, calibration0);
    }
    auto calibration90 = settings.value("calibration90").toString();
    if (!calibration90.isEmpty()) {
        openCalibrationFile(90, calibration90);
    }
}

ProjectModel::~ProjectModel() {
    QSettings settings;
    if (m_micCalibration0.startsWith("Error")) {
        m_micCalibration0.clear();
    }
    settings.setValue("calibration0", m_micCalibration0);
    if (m_micCalibration90.startsWith("Error")) {
        m_micCalibration90.clear();
    }
    settings.setValue("calibration90", m_micCalibration90);
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

QStringList ProjectModel::inputSampleRates() {
	return sampleRatesToStringList(m_inputSampleRates);
}

void ProjectModel::setInputSampleRate(int index) {
	m_inputSampleRateIndex = index;
	m_measurementService.setInputDevice(m_inputDevices.at(m_inputDeviceIndex), m_inputSampleRates.at(m_inputSampleRateIndex));
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

QStringList ProjectModel::outputSampleRates() {
	return sampleRatesToStringList(m_outputSampleRates);
}

void ProjectModel::setOutputSampleRate(int index) {
	m_outputSampleRateIndex = index;
	m_measurementService.setOutputDevice(m_outputDevices.at(m_outputDeviceIndex), m_outputSampleRates.at(m_outputSampleRateIndex));
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
