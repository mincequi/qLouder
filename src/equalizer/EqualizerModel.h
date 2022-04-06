#pragma once

#include <complex>
#include <charts/ChartModel.h>

class EqualizerModel : public ChartModel {
    Q_OBJECT

    Q_PROPERTY(QList<QObject*> filters READ filters NOTIFY filtersChanged)

    Q_PROPERTY(double sumMin MEMBER _sumMin NOTIFY rangeChanged)
    Q_PROPERTY(double sumMax MEMBER _sumMax NOTIFY rangeChanged)

public:
    explicit EqualizerModel(QObject *parent = nullptr);

    QObjectList filters() const;

public slots:
    void setSumSeries(QtCharts::QAbstractSeries* series);

    void addFilter(QtCharts::QAbstractSeries* response);
    void removeFilter(int index);

    void stepF(int index, int f);
    void stepQ(int index, double q);
    void stepG(int index, double g);

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

    std::vector<double> _frequencyTable;

    double _sumMax = -144.0;
    double _sumMin = 144.0;

    QtCharts::QXYSeries* _sumSeries = nullptr;

    QObjectList _filters;
};
