#pragma once

#include <complex>
#include <charts/ChartModel.h>

#include <rxcpp/rx-lite.hpp>

class TargetModel : public ChartModel {
    Q_OBJECT

    Q_PROPERTY(double loudness READ loudness NOTIFY valuesChanged)
    Q_PROPERTY(double harman MEMBER _harman NOTIFY valuesChanged)

    Q_PROPERTY(double sumMin MEMBER _sumMin NOTIFY valuesChanged)
    Q_PROPERTY(double sumMax MEMBER _sumMax NOTIFY valuesChanged)

public:
    explicit TargetModel(QObject *parent = nullptr);

    double loudness() const;

    rxcpp::observable<std::vector<double>> fr() const;

public slots:
    void setLoudnessSeries(QtCharts::QAbstractSeries* series);
    void setHarmanSeries(QtCharts::QAbstractSeries* series);
    void setSumSeries(QtCharts::QAbstractSeries* series);

    void moveHandle(int index, double x, double y) override;
    void stepParam(int index, double x, double y);
    void setLoudness(int phon);

signals:
    void valuesChanged();
    void frChanged(const std::vector<double>& response);

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

    std::vector<std::complex<double>> _loudnessResponse;
    QtCharts::QXYSeries* _loudnessSeries = nullptr;
    std::vector<std::complex<double>> _harmanResponse;
    QtCharts::QXYSeries* _harmanSeries = nullptr;
    rxcpp::subjects::subject<std::vector<double>> _fr;
    QtCharts::QXYSeries* _sumSeries = nullptr;
};
