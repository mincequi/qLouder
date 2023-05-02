#pragma once

#include <QObject>

class QAudioDeviceInfo;
class MeasurementService;

class SettingsModel : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString micCalibration0 MEMBER m_micCalibration0 NOTIFY modelChanged)
	Q_PROPERTY(QString micCalibration90 MEMBER m_micCalibration90 NOTIFY modelChanged)
	Q_PROPERTY(QStringList inputDevices READ inputDevices NOTIFY modelChanged)
    Q_PROPERTY(int savedInputDevice READ savedInputDevice)
    Q_PROPERTY(QStringList inputSampleRates READ inputSampleRates NOTIFY inputDeviceChanged)
    Q_PROPERTY(int savedInputSampleRate READ savedInputSampleRate)
	Q_PROPERTY(QStringList outputDevices READ outputDevices NOTIFY modelChanged)
    Q_PROPERTY(int savedOutputDevice READ savedOutputDevice)
    Q_PROPERTY(QStringList outputSampleRates READ outputSampleRates NOTIFY outputDeviceChanged)
    Q_PROPERTY(int savedOutputSampleRate READ savedOutputSampleRate)

public:
	explicit SettingsModel(MeasurementService& measurementService, QObject *parent = nullptr);
    ~SettingsModel();

	QStringList inputDevices();
	Q_INVOKABLE void setInputDevice(int index);
    int savedInputDevice() const;

	QStringList inputSampleRates();
	Q_INVOKABLE void setInputSampleRate(int index);
    int savedInputSampleRate() const;

	QStringList outputDevices();
	Q_INVOKABLE void setOutputDevice(int index);
    int savedOutputDevice() const;

    QStringList outputSampleRates();
	Q_INVOKABLE void setOutputSampleRate(int index);
    int savedOutputSampleRate() const;

    Q_INVOKABLE void openCalibrationFile(int degrees, const QString& fileName);

signals:
	void modelChanged();
	void inputDeviceChanged();
	void outputDeviceChanged();

private:
    void openInputDevice(const QAudioDeviceInfo& device);

	MeasurementService& m_measurementService;

	QString m_micCalibration0;
	QString m_micCalibration90;

	QList<QAudioDeviceInfo> m_inputDevices;
	int m_inputDeviceIndex = 0;
    const QList<int> m_inputSampleRates = { 44100, 48000 };
	int m_inputSampleRateIndex = 0;
	QList<QAudioDeviceInfo> m_outputDevices;
	int m_outputDeviceIndex = 0;
    const QList<int> m_outputSampleRates = { 44100, 48000 };
	int m_outputSampleRateIndex = 0;
};
