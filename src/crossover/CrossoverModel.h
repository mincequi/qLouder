#pragma once

#include <complex>
#include <charts/ChartModel.h>

class CrossoverModel : public ChartModel {
	Q_OBJECT

    Q_PROPERTY(std::vector<qreal> frequencies MEMBER _frequencyTable CONSTANT)

    Q_PROPERTY(double lowPassFrequencySlider READ lowPassFrequencySlider NOTIFY rangeChanged)
    Q_PROPERTY(QString lowPassFrequencyReadout READ lowPassFrequencyReadout NOTIFY rangeChanged)
    Q_PROPERTY(double lowPassQ READ lowPassQ NOTIFY rangeChanged)
    Q_PROPERTY(double lowPassG MEMBER _lowPassG NOTIFY rangeChanged)

    Q_PROPERTY(double highPassFrequencySlider READ highPassFrequencySlider NOTIFY rangeChanged)
    Q_PROPERTY(QString highPassFrequencyReadout READ highPassFrequencyReadout NOTIFY rangeChanged)
    Q_PROPERTY(double highPassQ READ highPassQ NOTIFY rangeChanged)
    Q_PROPERTY(double highPassG MEMBER _highPassG NOTIFY rangeChanged)

    Q_PROPERTY(double sumMin MEMBER _sumMin NOTIFY rangeChanged)
    Q_PROPERTY(double sumMax MEMBER _sumMax NOTIFY rangeChanged)

public:
    explicit CrossoverModel(QObject *parent = nullptr);

    double lowPassFrequencySlider() const;
    double highPassFrequencySlider() const;
    Q_INVOKABLE void setLowPassFrequencySlider(double value);
    Q_INVOKABLE void setHighPassFrequencySlider(double value);
    QString lowPassFrequencyReadout() const;
    double lowPassQ() const;
    QString highPassFrequencyReadout() const;
    double highPassQ() const;

public slots:
    virtual void setHandles(QtCharts::QAbstractSeries* series) override;
    void moveHandle(int index, double x, double y) override;
    void stepParam(int index, double x, double y);
    void setLowPassSeries(QtCharts::QAbstractSeries* series);
    void setHighPassSeries(QtCharts::QAbstractSeries* series);
    void setSumSeries(QtCharts::QAbstractSeries* series);

signals:
	void rangeChanged();

private:
    double yMin() const override;
    double yMax() const override;
    void computeLowPassResponse();
    void computeHighPassResponse();
    void computeSumResponse();

    std::vector<double> _frequencyTable;
    int _lowPassF = 0.0;
    double _lowPassQ = -3.0;
    double _lowPassG = 0.0;
    int _highPassF = 0.0;
    double _highPassQ = -3.0;
    double _highPassG = 0.0;
    double _sumMax = -144.0;
    double _sumMin = 144.0;

    QtCharts::QXYSeries* _handles = nullptr;
    std::vector<std::complex<double>> _lowPassResponse;
    QtCharts::QXYSeries* _lowPassSeries = nullptr;
    std::vector<std::complex<double>> _highPassResponse;
    QtCharts::QXYSeries* _highPassSeries = nullptr;
    QtCharts::QXYSeries* _sumSeries = nullptr;
};
