#ifndef MEASUREMENTMANAGER_H
#define MEASUREMENTMANAGER_H

#include <QObject>

template <class T>
class Measurement;

class MeasurementService;

class MeasurementManager : public QObject {
    Q_OBJECT
public:
    static MeasurementManager& instance();
    ~MeasurementManager();

    Measurement<float>* currentMeasurement() const;

signals:
    void currentMeasurementChangedF(Measurement<float>* measurement);
    void currentMeasurementChangedD(Measurement<double>* measurement);

private:
    explicit MeasurementManager(const MeasurementService& measurementService, QObject *parent = nullptr);
    void onMeasurementAvailable(Measurement<float>* measurement);

    const MeasurementService& _measurementService;
    std::vector<Measurement<float>*> _measurements;
    Measurement<float>* _currentMeasurement = nullptr;
};

#endif // MEASUREMENTMANAGER_H
