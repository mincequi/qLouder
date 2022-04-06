#pragma once

#include <complex>

#include <QObject>
#include <QtCharts/QXYSeries>

class FilterModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QtCharts::QAbstractSeries* response READ response CONSTANT)

    Q_PROPERTY(QString name2 MEMBER _name CONSTANT)
    Q_PROPERTY(QString f READ f NOTIFY valuesChanged)
    Q_PROPERTY(double q READ q NOTIFY valuesChanged)
    Q_PROPERTY(double g MEMBER _g NOTIFY valuesChanged)

public:
    explicit FilterModel(QtCharts::QAbstractSeries* response,
                         QObject *parent = nullptr);

    QString f() const;
    double q() const;

    QtCharts::QAbstractSeries* response() const;

    void moveHandle(int x, double y);
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
    int _f = 120;
    double _q = 3.0;
    double _g = -3.0;
    //std::vector<std::complex<double>> _response;

    friend class EqualizerModel;
};
