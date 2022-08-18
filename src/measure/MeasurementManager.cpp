#include "MeasurementManager.h"

#include "Measurement.h"
#include "MeasurementService.h"

#include <rxcpp/operators/rx-combine_latest.hpp>

MeasurementManager& MeasurementManager::instance() {
    static MeasurementManager instance(MeasurementService::instance());
    return instance;
}

MeasurementManager::MeasurementManager(const MeasurementService& measurementService, QObject *parent)
    : QObject{parent},
      _measurementService(measurementService) {
    connect(&measurementService, &MeasurementService::measurementAvailable,
            this, &MeasurementManager::onMeasurementAvailable);
}

MeasurementManager::~MeasurementManager() {
}

Measurement<float>* MeasurementManager::currentMeasurement() const {
    return _currentMeasurement;
}

void MeasurementManager::setFrCalibration(Calibration calibration) {
    if (!_currentMeasurement) return;

    _currentMeasurement->setFrCalibration(calibration);

    _calibratedFr.get_subscriber().on_next(_currentMeasurement->calibratedFr());
}

rxcpp::observable<std::vector<double>> MeasurementManager::calibratedFr() const {
    return _calibratedFr.get_observable();
}

void MeasurementManager::onMeasurementAvailable(Measurement<float>* measurement) {
    _measurements.push_back(measurement);
    _currentMeasurement = measurement;
    emit currentMeasurementChangedF(_currentMeasurement);

    _calibratedFr.get_subscriber().on_next(_currentMeasurement->calibratedFr());
}
