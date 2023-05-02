#pragma once

#include <QIODevice>

class AudioBuffer : public QIODevice {
	Q_OBJECT
public:
    explicit AudioBuffer(QIODevice::OpenMode mode, QObject* parent = nullptr);

	void start();
	void stop();
    float level();

	qint64 readData(char *data, qint64 maxlen) override;
	qint64 writeData(const char *data, qint64 len) override;

    std::vector<float> data;

signals:
    void ranEmpty();

private:
    QIODevice::OpenMode m_mode;
	size_t m_pos = 0;
    float m_level = 0.0f;
};
