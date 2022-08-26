#ifndef EQUALIZERLOGIC_H
#define EQUALIZERLOGIC_H

#include <QObject>
#include <QtCharts/QXYSeries>

#include <FrequencyTable.h>

class EqualizerLogic : public QObject
{
    Q_OBJECT
public:
    explicit EqualizerLogic(QObject *parent = nullptr);

public slots:
    //void addFilter(QtCharts::QAbstractSeries* response);
    //void removeFilter(int index);
    //void optimize();

signals:

private:
    FrequencyTable<double> _fTable;
};

#endif // EQUALIZERLOGIC_H
