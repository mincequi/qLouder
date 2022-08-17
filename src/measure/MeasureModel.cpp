#include "MeasureModel.h"

#include <QAudioDeviceInfo>
#include <QDebug>
#include <QSaveFile>
#include <QTimer>
#include <QUrl>
#include <QVariant>

#include "FrequencyTable.h"
#include "MeasurementService.h"

MeasureModel::MeasureModel(MeasurementService& measurementService, QObject *parent)
	: QObject(parent),
	  m_measurementService(measurementService) {
    FrequencyTable<double> table(3);
	m_frequencyTable = table.frequencies();
    m_maxFrequencySlider = m_frequencyTable.size()-1;
    connect(&m_measurementService, &MeasurementService::progressChanged, this, &MeasureModel::onServiceProgressChanged);
    connect(&m_measurementService, &MeasurementService::levelChanged, this, &MeasureModel::levelChanged);
    connect(&m_measurementService, &MeasurementService::errorOccured, this, &MeasureModel::errorOccurred);

    //QTimer *timer = new QTimer(this);
    //connect(timer, &QTimer::timeout, this, [this]() {
    //    m_demoProgress += 0.1f;
    //    m_demoLevel -= 0.1f;
    //    emit progressChanged();
    //    emit levelChanged();
    //});
    //timer->start(100);
}

QStringList MeasureModel::lengths() const {
    return { "¼ s", "½ s", "1 s", "2 s", "4 s" };
}

QStringList MeasureModel::channels() const {
    return { "Left", "Right", "Stereo" };
}

void MeasureModel::setLength(int index) {
	switch (index) {
    case 0: m_durationPerOctave = 250; break;
    case 1: m_durationPerOctave = 500; break;
    case 2: m_durationPerOctave = 1000; break;
    case 3: m_durationPerOctave = 2000; break;
    case 4: m_durationPerOctave = 4000; break;
	default:
		break;
	}
}

void MeasureModel::setChannels(int index) {
    _channels = static_cast<Signal::Channels>(index);
}

void MeasureModel::setLevel(int value) {
    m_level = value;
}

double MeasureModel::minFrequencySlider() const {
	return m_minFrequencySlider;
}

double MeasureModel::maxFrequencySlider() const {
	return m_maxFrequencySlider;
}

void MeasureModel::setMinFrequencySlider(double value) {
    m_minFrequencySlider = std::min(value, (double)(m_frequencyTable.size()-2));
	if (m_maxFrequencySlider <= m_minFrequencySlider) {
		m_maxFrequencySlider = m_minFrequencySlider + 1;
	}
	emit rangeChanged();
}

void MeasureModel::setMaxFrequencySlider(double value) {
	m_maxFrequencySlider = std::max(value, 1.0);
	if (m_minFrequencySlider >= m_maxFrequencySlider) {
		m_minFrequencySlider = m_maxFrequencySlider - 1;
	}
	emit rangeChanged();
}

QString MeasureModel::minFrequencyReadout() const {
	const auto f = m_frequencyTable.at(m_minFrequencySlider);
    if (f > 3500.0) {
		return QString::number(f/1000.0, 'f', 0) + " kHz";
    } else if (f > 900.0) {
		return QString::number(f/1000.0, 'f', 1) + " kHz";
	} else {
		return QString::number(f, 'f', 0) + " Hz";
	}
}

QString MeasureModel::maxFrequencyReadout() const {
	const auto f = m_frequencyTable.at(m_maxFrequencySlider);
    if (f > 3500.0) {
		return QString::number(f/1000.0, 'f', 0) + " kHz";
    } else if (f > 900.0) {
		return QString::number(f/1000.0, 'f', 1) + " kHz";
	} else {
		return QString::number(f, 'f', 0) + " Hz";
	}
}

double MeasureModel::progress() const {
	return m_measurementService.progress();
}

double MeasureModel::level() const {
    return m_measurementService.level();
}

double MeasureModel::levelMax() const {
    return m_measurementService.levelMax();
}

QString MeasureModel::errorString() const {
    return m_measurementService.errorString();
}

QString MeasureModel::errorDescription() const {
    return m_measurementService.errorDescription();
}

QString MeasureModel::measureButtonIcon() const {
	switch (m_state) {
	case State::Idle:
		return "circle";
	case State::Measuring:
		return "close-circle";
	}
}

QString MeasureModel::measureButtonText() const {
	switch (m_state) {
	case State::Idle:
		return "Measure";
	case State::Measuring:
		return "Abort";
	}
}

void MeasureModel::onMeasureButtonClicked() {
	switch (m_state) {
	case State::Idle:
		m_state = State::Measuring;
        m_measurementService.start(_channels, m_durationPerOctave, m_level, m_frequencyTable.at(m_minFrequencySlider), m_frequencyTable.at(m_maxFrequencySlider));
		break;
	case State::Measuring:
		m_measurementService.stop();
		m_state = State::Idle;
		break;
	}

	emit measureStateChanged();
}

void MeasureModel::saveFile(const QUrl& fileName) {
    QSaveFile file(fileName.fileName());
    if (!file.open(QIODevice::WriteOnly)) return;
    const auto bytesWritten = file.write((const char*)m_measurementService.signal().data(), m_measurementService.signal().size()*4);
    if (!file.commit() || !bytesWritten) {
        qDebug() << "error writing file";
    }
}

void MeasureModel::onServiceProgressChanged(double progress) {
    //qDebug() << "measure> progress: " << progress;
    emit progressChanged();
    if (progress == 0.0) {
        m_state = State::Idle;
        emit measureStateChanged();
    }
}
