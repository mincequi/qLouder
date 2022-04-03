#pragma once

#include <complex>
#include <charts/ChartModel.h>

class TargetModel : public ChartModel {
    Q_OBJECT

    Q_PROPERTY(double loudness READ loudness NOTIFY valuesChanged)
    Q_PROPERTY(double harman MEMBER _harman NOTIFY valuesChanged)

    Q_PROPERTY(double sumMin MEMBER _sumMin NOTIFY valuesChanged)
    Q_PROPERTY(double sumMax MEMBER _sumMax NOTIFY valuesChanged)

public:
    explicit TargetModel(QObject *parent = nullptr);

    double loudness() const;

public slots:
    virtual void setHandles(QtCharts::QAbstractSeries* series) override;
    void setLoudnessSeries(QtCharts::QAbstractSeries* series);
    void setHarmanSeries(QtCharts::QAbstractSeries* series);
    void setSumSeries(QtCharts::QAbstractSeries* series);

    void moveHandle(int index, double x, double y) override;
    void stepParam(int index, double x, double y);

signals:
    void valuesChanged();

private:
    double yMin() const override;
    double yMax() const override;
    void computeLoudnessResponse();
    void computeHarmanResponse();
    void computeSumResponse();

    std::vector<double> _frequencyTable;
    double _loudness = 0.0;
    double _harman = 0.0;

    double _sumMax = -144.0;
    double _sumMin = 144.0;

    QtCharts::QXYSeries* _handles = nullptr;
    std::vector<std::complex<double>> _loudnessResponse;
    QtCharts::QXYSeries* _loudnessSeries = nullptr;
    std::vector<std::complex<double>> _harmanResponse;
    QtCharts::QXYSeries* _harmanSeries = nullptr;
    QtCharts::QXYSeries* _sumSeries = nullptr;
};
