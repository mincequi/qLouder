#pragma once

#include <map>

#include <QObject>

#include "Types.h"

#define cfg Config::instance()

class Config : public QObject {
	Q_OBJECT

public:
	static Config* instance();
	virtual ~Config();

	const QString& workDir() const;

    std::map<double, double> calibration0;
    std::map<double, double> calibration90;
    Calibration calibration;

signals:
	void workDirChanged(const QString& workDir);
	void audioConfigChanged();

private:
	explicit Config(QObject* parent = nullptr);

	void setWorkDir(const QString& workDir);

	void setSampleRate(int sampleRate);
	void setInputDevice(const QString& device);
	void setOutputDevice(const QString& device);

	QString fileName();

	QString m_workDir;
	int m_sampleRate = 48000;

	friend class ProjectModel;
};
