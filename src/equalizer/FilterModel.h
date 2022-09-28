#pragma once

#include <complex>

#include <QObject>
#include <QtCharts/QXYSeries>

#include "AudioFilter.h"

class FilterModel : public QObject {
    Q_OBJECT

    // Filter model needs to store response series, so that it can be identified
    // and removed from the chart when this filter gets deleted.
    Q_PROPERTY(QtCharts::QAbstractSeries* response READ response CONSTANT)

    Q_PROPERTY(int type READ type NOTIFY valuesChanged)
    Q_PROPERTY(QString f READ fAsString NOTIFY valuesChanged)
    Q_PROPERTY(QString fUnit READ fUnit NOTIFY valuesChanged)
    Q_PROPERTY(double q READ q NOTIFY valuesChanged)
    Q_PROPERTY(double g MEMBER _g NOTIFY valuesChanged)
    Q_PROPERTY(bool isGainEnabled READ isGainEnabled NOTIFY valuesChanged)

public:
    explicit FilterModel(FilterType type, int f, double q, double g,
                         QtCharts::QAbstractSeries* response,
                         QObject *parent = nullptr);

    int type() const;
    QString fAsString() const;
    QString fUnit() const;
    double f() const;
    double q() const;
    double g() const;
    bool isGainEnabled() const;

    QtCharts::QAbstractSeries* response() const;

    void moveHandle(int x, double y);
    void setType(FilterType type);
    void setQ(double q);

signals:
    void nameChanged();
    void valuesChanged();

private:
    void stepF(int f);
    void stepQ(double q);
    void stepG(double g);

    void computeResponse();

    QString _name = "BLAH2";

    QtCharts::QXYSeries* _response = nullptr;

    std::vector<double> _frequencyTable;
    FilterType _type = FilterType::Peak;
    int _f = 120;
    double _qIndex = 3.0;
    double _g = -3.0;
    //std::vector<std::complex<double>> _response;

    friend class EqualizerModel;
};
