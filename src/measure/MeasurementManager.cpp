#include "MeasurementManager.h"

#include "Measurement.h"
#include "MeasurementService.h"

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

void MeasurementManager::onMeasurementAvailable(Measurement<float>* measurement) {
    _measurements.push_back(measurement);
    _currentMeasurement = measurement;
    emit currentMeasurementChangedF(_currentMeasurement);
}
