#include "AudioDeviceInfo.h"

#include <QAudioDeviceInfo>

QString AudioDeviceInfo::name() const {
	return m_name;
}

AudioDeviceInfo AudioDeviceInfo::fromQAudioDeviceInfo(const QAudioDeviceInfo& in) {
	AudioDeviceInfo out;
	out.m_name = in.deviceName();

	return out;
}
