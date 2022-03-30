#include "AudioBuffer.h"

#include <assert.h>
#include <cmath>
#include <cstring>

#include <QDebug>

AudioBuffer::AudioBuffer(QIODevice::OpenMode mode, QObject* parent)
    : QIODevice(parent),
      m_mode(mode) {
}

void AudioBuffer::start() {
    m_pos = 0;
    open(m_mode);
}

void AudioBuffer::stop() {
	close();
}

float AudioBuffer::level() {
    float temp = log10(m_level);
    temp *= 20.0f;
    m_level = 0.0f;

    return temp;
}

qint64 AudioBuffer::readData(char* _data, qint64 maxlen) {
	assert(maxlen % 4 == 0);

	const auto len = std::min(static_cast<size_t>(maxlen), (data.size()-m_pos)*4);
	std::memcpy(_data, data.data()+m_pos, len);
	m_pos += len/4;

    if (len < 1) {
        emit ranEmpty();
    }

	return len;
}

qint64 AudioBuffer::writeData(const char* _data, qint64 len) {
	assert(len % 4 == 0);

	data.resize(m_pos + len/4);
    std::memcpy(data.data()+m_pos, _data, len);
    for (size_t i = m_pos; i < data.size(); ++i) {
        m_level = std::max(m_level, abs(data.at(i)));
    }

	m_pos += len/4;

	return len;
}
