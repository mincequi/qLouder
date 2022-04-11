#ifndef MEASUREMENTMANAGER_H
#define MEASUREMENTMANAGER_H

#include <QObject>

#include <Types.h>

#include <rxcpp/rx-lite.hpp>

template <class T>
class Measurement;

class MeasurementService;

class MeasurementManager : public QObject {
    Q_OBJECT
public:
    static MeasurementManager& instance();
    ~MeasurementManager();

    Measurement<float>* currentMeasurement() const;

    void setFrCalibration(Calibration calibration);

    rxcpp::observable<std::vector<double>> calibratedFr() const;

signals:
    void currentMeasurementChangedF(Measurement<float>* measurement);
    void currentMeasurementChangedD(Measurement<double>* measurement);

    void calibratedFrChanged(const std::vector<double>& fr);

private:
    explicit MeasurementManager(const MeasurementService& measurementService, QObject *parent = nullptr);
    void onMeasurementAvailable(Measurement<float>* measurement);

    const MeasurementService& _measurementService;
    std::vector<Measurement<float>*> _measurements;
    Measurement<float>* _currentMeasurement = nullptr;

    rxcpp::subjects::subject<std::vector<double>> _calibratedFr;
};

#endif // MEASUREMENTMANAGER_H
