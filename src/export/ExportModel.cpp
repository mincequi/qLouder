#include "ExportModel.h"

ExportModel::ExportModel(const EqualizerModel& eq, QObject *parent) : QObject(parent) {
    _pulseEffectsExporter = PulseEffectsExporter::create(eq);
}

bool ExportModel::isPulseEffectsAvailable() const {
    return (bool)_pulseEffectsExporter;
}

bool ExportModel::isPulseEffectsBound() const {
    return _pulseEffectsExporter ? _pulseEffectsExporter->isBound() : false;
}

void ExportModel::exportToPulseEffects() {
    if (_pulseEffectsExporter) {
        _pulseEffectsExporter->save();
    }
}

void ExportModel::bindToPulseEffects(bool bind) {
    if (!_pulseEffectsExporter) return;

    _pulseEffectsExporter->bind(bind);
    emit exportersChanged();
}
