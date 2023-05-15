#pragma once

#include <QObject>

#include <Types.h>

#include <rxcpp/rx-lite.hpp>

class Measurement;

class MeasurementService;

class MeasurementManager : public QObject {
    Q_OBJECT
public:
    static MeasurementManager& instance();
    ~MeasurementManager();

    Measurement* currentMeasurement() const;

    void setFrCalibration(Calibration calibration);

    rxcpp::observable<std::vector<double>> calibratedFr() const;

signals:
    void currentMeasurementChangedF(Measurement* measurement);

private:
    explicit MeasurementManager(const MeasurementService& measurementService, QObject *parent = nullptr);
    void onMeasurementAvailable(Measurement* measurement);

    const MeasurementService& _measurementService;
    std::vector<Measurement*> _measurements;
    Measurement* _currentMeasurement = nullptr;

    rxcpp::subjects::subject<std::vector<double>> _calibratedFr;
};
