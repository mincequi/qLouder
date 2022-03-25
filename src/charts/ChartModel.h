#ifndef CHARTMODEL_H
#define CHARTMODEL_H

#include <memory>

#include <QtCharts/QXYSeries>

#include "AbstractChart.h"
#include "measure/Measurement.h"

class ChartModel : public QObject {
    Q_OBJECT
    // General
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)

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
    double yMin() const;
    double yMax() const;
    int tickInterval() const;
    Q_INVOKABLE void zoom(double zoom);
    Q_INVOKABLE void pan(double pan);

signals:
    void typeChanged();
    void measurementChanged();
    void rangeChanged();
    void irWindowChanged();

private:
    void onCurrentMeasurementChanged(Measurement<float>* measurement);

    Type _type = ImpulseResponse;
    // Use unique_ptr here to prevent object slicinig
    std::map<Type, std::unique_ptr<AbstractChart>> _charts;
};

#endif // CHARTMODEL_H
