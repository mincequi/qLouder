#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QObject>

class QAudioDeviceInfo;
class MeasurementService;

class ProjectModel : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString projectDir MEMBER m_projectDir NOTIFY modelChanged)
	Q_PROPERTY(QString micCalibration0 MEMBER m_micCalibration0 NOTIFY modelChanged)
	Q_PROPERTY(QString micCalibration90 MEMBER m_micCalibration90 NOTIFY modelChanged)
	Q_PROPERTY(QStringList inputDevices READ inputDevices NOTIFY modelChanged)
	Q_PROPERTY(QStringList inputSampleRates READ inputSampleRates NOTIFY inputDeviceChanged)
	Q_PROPERTY(QStringList outputDevices READ outputDevices NOTIFY modelChanged)
	Q_PROPERTY(QStringList outputSampleRates READ outputSampleRates NOTIFY outputDeviceChanged)

public:
	explicit ProjectModel(MeasurementService& measurementService, QObject *parent = nullptr);
    ~ProjectModel();

	QStringList inputDevices();
	Q_INVOKABLE void setInputDevice(int index);
	QStringList inputSampleRates();
	Q_INVOKABLE void setInputSampleRate(int index);
	QStringList outputDevices();
	Q_INVOKABLE void setOutputDevice(int index);
	QStringList outputSampleRates();
	Q_INVOKABLE void setOutputSampleRate(int index);

    Q_INVOKABLE void openCalibrationFile(int degrees, const QString& fileName);

signals:
	void modelChanged();
	void inputDeviceChanged();
	void outputDeviceChanged();

private:
    static void openInputDevice(const QAudioDeviceInfo& device);

	MeasurementService& m_measurementService;

	QString m_projectDir;
	QString m_micCalibration0;
	QString m_micCalibration90;

	QList<QAudioDeviceInfo> m_inputDevices;
	int m_inputDeviceIndex = 0;
	QList<int> m_inputSampleRates;
	int m_inputSampleRateIndex = 0;
	QList<QAudioDeviceInfo> m_outputDevices;
	int m_outputDeviceIndex = 0;
	QList<int> m_outputSampleRates;
	int m_outputSampleRateIndex = 0;
};

#endif // PROJECTMODEL_H
