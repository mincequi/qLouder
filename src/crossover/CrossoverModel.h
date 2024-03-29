#pragma once

#include <complex>
#include <charts/ChartModel.h>

class CrossoverModel : public ChartModel {
	Q_OBJECT

    Q_PROPERTY(QString lowPassFrequencyReadout READ lowPassFrequencyReadout NOTIFY rangeChanged)
    Q_PROPERTY(QString lowPassFrequencyUnitReadout READ lowPassFrequencyUnitReadout NOTIFY rangeChanged)
    Q_PROPERTY(double lowPassQ READ lowPassQ NOTIFY rangeChanged)
    Q_PROPERTY(double lowPassG MEMBER _lowPassG NOTIFY rangeChanged)
    Q_PROPERTY(int lowPassOrder MEMBER _lowPassOrderIndex NOTIFY rangeChanged)

    Q_PROPERTY(QString highPassFrequencyReadout READ highPassFrequencyReadout NOTIFY rangeChanged)
    Q_PROPERTY(QString highPassFrequencyUnitReadout READ highPassFrequencyUnitReadout NOTIFY rangeChanged)
    Q_PROPERTY(double highPassQ READ highPassQ NOTIFY rangeChanged)
    Q_PROPERTY(double highPassG MEMBER _highPassG NOTIFY rangeChanged)
    Q_PROPERTY(int highPassOrder MEMBER _highPassOrderIndex NOTIFY rangeChanged)
    Q_PROPERTY(int highPassInvert MEMBER _invert NOTIFY rangeChanged)

    Q_PROPERTY(double sumMin MEMBER _sumMin NOTIFY rangeChanged)
    Q_PROPERTY(double sumMax MEMBER _sumMax NOTIFY rangeChanged)
    Q_PROPERTY(QString ripple READ ripple NOTIFY rangeChanged)

public:
    explicit CrossoverModel(QObject *parent = nullptr);

    QString lowPassFrequencyReadout() const;
    QString lowPassFrequencyUnitReadout() const;
    double lowPassQ() const;
    QString highPassFrequencyReadout() const;
    QString highPassFrequencyUnitReadout() const;
    double highPassQ() const;

    QString ripple() const;

public slots:
    virtual void setHandles(QtCharts::QAbstractSeries* series) override;
    void setLowPassSeries(QtCharts::QAbstractSeries* series);
    void setHighPassSeries(QtCharts::QAbstractSeries* series);
    void setSumSeries(QtCharts::QAbstractSeries* series);

    void moveHandle(int index, double x, double y) override;
    void stepParam(int index, double x, double y);
    void setOrder(int index, int orderIndex);
    void invert(bool invert);

signals:
    void rangeChanged();
    void frChanged(const std::vector<double>& response);

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
    int _lowPassOrderIndex = 0;
    int _highPassF = 0.0;
    double _highPassQ = -3.0;
    double _highPassG = 0.0;
    int _highPassOrderIndex = 0;
    bool _invert = true;

    double _sumMax = -144.0;
    double _sumMin = 144.0;

    std::vector<std::complex<double>> _lowPassResponse;
    QtCharts::QXYSeries* _lowPassSeries = nullptr;
    std::vector<std::complex<double>> _highPassResponse;
    QtCharts::QXYSeries* _highPassSeries = nullptr;
    std::vector<double> _fr;
    QtCharts::QXYSeries* _sumSeries = nullptr;

    friend class StatusModel;
};
