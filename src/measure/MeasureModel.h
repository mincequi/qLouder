#ifndef MEASUREMODEL_H
#define MEASUREMODEL_H

#include <QAudio>
#include <QObject>

#include <vector>

#include "Signal.h"

class MeasurementService;

class MeasureModel : public QObject {
	Q_OBJECT
    // Signal properties
	Q_PROPERTY(QStringList lengths READ lengths CONSTANT)
    Q_PROPERTY(QStringList channels READ channels CONSTANT)
    Q_PROPERTY(std::vector<qreal> frequencies MEMBER m_frequencyTable CONSTANT)
	Q_PROPERTY(double minFrequencySlider READ minFrequencySlider NOTIFY rangeChanged)
	Q_PROPERTY(double maxFrequencySlider READ maxFrequencySlider NOTIFY rangeChanged)
	Q_PROPERTY(QString minFrequencyReadout READ minFrequencyReadout NOTIFY rangeChanged)
	Q_PROPERTY(QString maxFrequencyReadout READ maxFrequencyReadout NOTIFY rangeChanged)

    // Microphone calibration
    Q_PROPERTY(int calibration MEMBER _calibration NOTIFY calibrationChanged)

    // Measure button properties
	Q_PROPERTY(QString measureButtonIcon READ measureButtonIcon NOTIFY measureStateChanged)
	Q_PROPERTY(QString measureButtonText READ measureButtonText NOTIFY measureStateChanged)

    // Current measurement properties
	Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(double level READ level NOTIFY levelChanged)
    Q_PROPERTY(double levelMax READ levelMax NOTIFY levelChanged)

    // Error handling
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorOccurred)
    Q_PROPERTY(QString errorDescription READ errorDescription NOTIFY errorOccurred)

public:
	explicit MeasureModel(MeasurementService& measurementService, QObject *parent = nullptr);

    // Signal properties
	QStringList lengths() const;
	Q_INVOKABLE void setLength(int index);
    QStringList channels() const;
    Q_INVOKABLE void setChannels(int index);
    Q_INVOKABLE void setLevel(int value);
	double minFrequencySlider() const;
	double maxFrequencySlider() const;
	Q_INVOKABLE void setMinFrequencySlider(double value);
	Q_INVOKABLE void setMaxFrequencySlider(double value);
	QString minFrequencyReadout() const;
	QString maxFrequencyReadout() const;

    QString measureButtonIcon() const;
	QString measureButtonText() const;
	Q_INVOKABLE void onMeasureButtonClicked();

    Q_INVOKABLE void saveFile(const QUrl& fileName);
	double progress() const;
    double level() const;
    double levelMax() const;
    QString errorString() const;
    QString errorDescription() const;

signals:
	void rangeChanged();
	void progressChanged();
    void levelChanged();
    void calibrationChanged();
	void measureStateChanged();
    void errorOccurred();

private:
    void onServiceProgressChanged(double progress);
    int m_durationPerOctave = 250;
    Signal::Channels _channels = Signal::Channels::Left;
    int m_level = 0;
    std::vector<double> m_frequencyTable;
	uint m_minFrequencySlider = 0;
	uint m_maxFrequencySlider = 10;

    int _calibration = -1;

	enum class State {
		Idle,
		Measuring
	};
	State m_state = State::Idle;
    QString m_error;
    QString m_errorDescription;

	MeasurementService& m_measurementService;
};

#endif // MEASUREMODEL_H
