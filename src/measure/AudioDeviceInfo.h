#ifndef AUDIODEVICEINFO_H
#define AUDIODEVICEINFO_H

#include <QObject>

class QAudioDeviceInfo;

class AudioDeviceInfo {
	Q_GADGET
	Q_PROPERTY(QString name READ name)

public:
	static AudioDeviceInfo fromQAudioDeviceInfo(const QAudioDeviceInfo& in);

	QString name() const;

private:
	QString m_name;
};

Q_DECLARE_METATYPE(AudioDeviceInfo)

#endif // AUDIODEVICEINFO_H
