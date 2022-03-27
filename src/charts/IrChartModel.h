#ifndef IRCHARTMODEL_H
#define IRCHARTMODEL_H

#include "ChartModel.h"

class IrChartModel : public ChartModel {
    Q_OBJECT

    Q_PROPERTY(bool hasMeasurement READ hasMeasurement NOTIFY measurementChanged)

    // IR window
    Q_PROPERTY(QString irWindowLeftReadout READ irWindowLeftReadout NOTIFY irWindowChanged)
    Q_PROPERTY(QString irWindowRightReadout READ irWindowRightReadout NOTIFY irWindowChanged)
    Q_PROPERTY(double irWindowLeft READ irWindowLeft NOTIFY irWindowChanged)
    Q_PROPERTY(double irWindowRight READ irWindowRight NOTIFY irWindowChanged)

public:
    explicit IrChartModel(QObject *parent = nullptr);

    Q_INVOKABLE void setSeries(QtCharts::QAbstractSeries* series);
    Q_INVOKABLE void setIrWindowSeries(QtCharts::QAbstractSeries* series);
    Q_INVOKABLE void setIrWindowHandles(QtCharts::QAbstractSeries* series);
    Q_INVOKABLE void setUpperEnvelopeSeries(QtCharts::QAbstractSeries* series);
    Q_INVOKABLE void setLowerEnvelopeSeries(QtCharts::QAbstractSeries* series);

    bool hasMeasurement() const;

    // IR window
    QString irWindowLeftReadout() const;
    QString irWindowRightReadout() const;
    double irWindowLeft() const;
    double irWindowRight() const;
    Q_INVOKABLE void moveIrWindowLeft(int step);
    Q_INVOKABLE void moveIrWindowRight(int step);
    void moveHandle(int index, double x, double y) override;

signals:

private:
    void onCurrentMeasurementChangedF(Measurement<float>* measurement);

    void updateChart();
    void updateIrWindow();

    static QVector<QPointF> simplify(const QVector<QPointF>& in);

    QtCharts::QXYSeries* _series = nullptr;

    QtCharts::QXYSeries* _irWindowSeries = nullptr;
    QtCharts::QXYSeries* _irWindowHandles = nullptr;
    QtCharts::QXYSeries* _upperEnvelopeSeries = nullptr;
    QtCharts::QXYSeries* _lowerEnvelopeSeries = nullptr;

    Measurement<float>* _measurement = nullptr;

};

#endif // IRCHARTMODEL_H
