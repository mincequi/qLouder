#pragma once

#include <memory>

#include <QtCharts/QXYSeries>

#include "AbstractChart.h"
#include "measure/Measurement.h"

class ChartModel : public QObject {
    Q_OBJECT
    // General
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QtCharts::QXYSeries* handles MEMBER _handles CONSTANT)

    // Zoom, scroll
    Q_PROPERTY(double xMin READ xMin NOTIFY rangeChanged)
    Q_PROPERTY(double xMax READ xMax NOTIFY rangeChanged)
    Q_PROPERTY(double yMin READ yMin NOTIFY rangeChanged)
    Q_PROPERTY(double yMax READ yMax NOTIFY rangeChanged)
    Q_PROPERTY(int tickInterval READ tickInterval NOTIFY rangeChanged)

    //Q_PROPERTY(bool isZoomInEnabled READ isZoomInEnabled NOTIFY rangeChanged)

public:
    enum Type {
        ImpulseResponse,
        FrequencyResponse
    };
    Q_ENUM(Type)

    explicit ChartModel(QObject *parent = nullptr);

    // General, initializers
    void setType(Type);
    Type type() const;

    // Zoom, scroll
    double xMin() const;
    double xMax() const;
    virtual double yMin() const;
    virtual double yMax() const;
    int tickInterval() const;
    Q_INVOKABLE void zoom(double zoom);
    Q_INVOKABLE void pan(double pan);

public slots:
    virtual void setHandles(QtCharts::QAbstractSeries* series);
    virtual void moveHandle(int index, double x, double y);

signals:
    void typeChanged();
    void measurementChanged();
    void rangeChanged();
    void irWindowChanged();

protected:
    virtual QtCharts::QXYSeries* handles();

private:
    Type _type = ImpulseResponse;
    QtCharts::QXYSeries* _handles = nullptr;

    // Use unique_ptr here to prevent object slicinig
    std::map<Type, std::unique_ptr<AbstractChart>> _charts;
};
