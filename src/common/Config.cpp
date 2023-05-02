#include "Config.h"

Config* Config::instance() {
	static Config instance;
	return &instance;
}

Config::Config(QObject* parent) {
}

Config::~Config() {
}

const QString& Config::workDir() const {
	return m_workDir;
}

void Config::setWorkDir(const QString& workDir) {
	m_workDir = workDir;
	emit workDirChanged(workDir);
}

void Config::setSampleRate(int sampleRate) {
	m_sampleRate = sampleRate;
	emit audioConfigChanged();
}

void Config::setInputDevice(const QString& device) {
	emit audioConfigChanged();
}

void Config::setOutputDevice(const QString& device) {
	emit audioConfigChanged();
}

QString Config::fileName() {
	return this->m_workDir + "/qloud.xml";
}
