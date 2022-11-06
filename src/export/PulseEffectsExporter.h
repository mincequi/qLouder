#pragma once

#include <memory>

#include <QObject>

class QGSettings;
class EqualizerModel;

class PulseEffectsExporter : public QObject {
    Q_OBJECT

public:
    static std::unique_ptr<PulseEffectsExporter> create(const EqualizerModel& eq);

    void save();
    void bind(bool bind_);
    bool isBound() const;

private:
    PulseEffectsExporter(const EqualizerModel& eq);

    void onValueChanged(const QString& key);
    void onLeftValueChanged(const QString& key);
    void onRightValueChanged(const QString& key);

    void doExportGeneral();
    void doExportFilters();

    const EqualizerModel& _eq;

    QGSettings* _settings = nullptr;
    QGSettings* _leftSettings = nullptr;
    QGSettings* _rightSettings = nullptr;

    bool _isBound = false;
};
