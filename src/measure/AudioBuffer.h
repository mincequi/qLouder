#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <QIODevice>
#include <QVector>
#include <QtMultimedia/QAudioBuffer>

class AudioBuffer : public QIODevice {
	Q_OBJECT
public:
    explicit AudioBuffer(QIODevice::OpenMode mode, QObject* parent = nullptr);

	void start();
	void stop();

    std::vector<float> data;

    float level();

	qint64 readData(char *data, qint64 maxlen) override;
	qint64 writeData(const char *data, qint64 len) override;

signals:
    void ranEmpty();

private:
    QIODevice::OpenMode m_mode;
	size_t m_pos = 0;
    float m_level = 0.0f;
};

#endif // AUDIOBUFFER_H
