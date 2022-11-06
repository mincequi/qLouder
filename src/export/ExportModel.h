#pragma once

#include <memory>

#include <QObject>

#include "PulseEffectsExporter.h"

class ExportModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool isPulseEffectsAvailable READ isPulseEffectsAvailable NOTIFY exportersChanged)
    Q_PROPERTY(bool isPulseEffectsBound READ isPulseEffectsBound NOTIFY exportersChanged)

public:
    explicit ExportModel(const EqualizerModel& eq, QObject *parent = nullptr);

    bool isPulseEffectsAvailable() const;
    bool isPulseEffectsBound() const;

public slots:
    void exportToPulseEffects();
    void bindToPulseEffects(bool bind);

signals:
    void exportersChanged();

private:
    std::unique_ptr<PulseEffectsExporter> _pulseEffectsExporter;
};
