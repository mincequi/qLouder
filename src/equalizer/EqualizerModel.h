#pragma once

#include <complex>
#include <charts/ChartModel.h>

#include <rxcpp/rx-lite.hpp>

class TargetModel;

class EqualizerModel : public ChartModel {
    Q_OBJECT

    Q_PROPERTY(std::vector<qreal> frequencies MEMBER _rangeTable CONSTANT)
    Q_PROPERTY(double minFrequencySlider READ minFrequencySlider NOTIFY rangeChanged)
    Q_PROPERTY(double maxFrequencySlider READ maxFrequencySlider NOTIFY rangeChanged)
    Q_PROPERTY(QString minFrequencyReadout READ minFrequencyReadout NOTIFY rangeChanged)
    Q_PROPERTY(QString maxFrequencyReadout READ maxFrequencyReadout NOTIFY rangeChanged)

    Q_PROPERTY(QList<QObject*> filters READ filters NOTIFY filtersChanged)

    Q_PROPERTY(double sumMin MEMBER _sumMin NOTIFY rangeChanged)
    Q_PROPERTY(double sumMax MEMBER _sumMax NOTIFY rangeChanged)

public:
    explicit EqualizerModel(const TargetModel& targetModel, QObject *parent = nullptr);

    double minFrequencySlider() const;
    double maxFrequencySlider() const;
    QString minFrequencyReadout() const;
    QString maxFrequencyReadout() const;

    QObjectList filters() const;

public slots:
    void setFilterSumSeries(QtCharts::QAbstractSeries* series);
    void setTargetSeries(QtCharts::QAbstractSeries* series);
    void setFilteredMeasurementSeries(QtCharts::QAbstractSeries* series);

    void addFilter(QtCharts::QAbstractSeries* response);
    void removeFilter(int index);
    void optimize();

    void stepF(int index, int f);
    void stepQ(int index, double q);
    void stepG(int index, double g);

    void setLevel(int value);
    void setMinFrequencySlider(double value);
    void setMaxFrequencySlider(double value);

signals:
    void filtersChanged();
    void rangeChanged();

private:
    double yMin() const override;
    double yMax() const override;
    void moveHandle(int index, double x, double y) override;
    void moveLeftQHandle(int index, double x);
    void moveRightQHandle(int index, double x);

    void computeSumResponse();
    bool findMaxOvershoot(int* f, double* g);
    void findQ(int f, double g, double* q);
    double deviationWithFilter(int f, double g, int q);
    double deviation();

    const TargetModel& _targetModel;
    std::vector<double> _frequencyTable;
    std::vector<double> _rangeTable;
    rxcpp::subjects::subject<int> _level;

    double _sumMax = -144.0;
    double _sumMin = 144.0;

    QtCharts::QXYSeries* _sumSeries = nullptr;
    std::vector<double> _target;
    QtCharts::QXYSeries* _targetSeries = nullptr;
    std::vector<double> _filtered;
    QtCharts::QXYSeries* _filteredSeries = nullptr;

    QObjectList _filters;

    rxcpp::subjects::subject<std::vector<double>> _sum;
    rxcpp::subjects::behavior<std::pair<int,int>> _range;

    rxcpp::observable<QtCharts::QXYSeries*> _filteredMeasurement;
};
