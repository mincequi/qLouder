#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <functional>

#include "FrequencyTable.h"
#include "Types.h"

/**
 * @brief The Measurement class
 *
 * This is a passive data class to hold any values to save and restore a
 * measurement from disk. It shall also encapsulate "basic" and "raw" data
 * operation/manipulation like windowing and FFT.
 *
 * @tparam T the data type to be used for measurement: double or float.
 */
template<class T>
class Measurement {
public:
    explicit Measurement(int sampleRate,
                         double fMin,
                         double fMax,
                         const std::vector<T>& inputSignal,
                         const std::vector<T>& inverseFilter,
                         const std::map<double, double>& calibration0 = {},
                         const std::map<double, double>& calibration90 = {},
                         Calibration calibration = CalibrationNone);

    const std::vector<T>& irWindow();
    const std::vector<T>& windowedIr();

    double irWindowLeft() const;
    void setIrWindowLeft(double ms);

    double irWindowRight() const;
    void setIrWindowRight(double ms);

    // Frequency response related methods
    const std::vector<double>& frequencies();
    void setFrChangedCallback(std::function<void()>);

    void setFrCalibration(Calibration calibration);
    const std::vector<double>& calibratedFr();
    const std::vector<double>& frCalibration(Calibration calibration);

private:
    const std::vector<double>& fr();

    const std::vector<T>& ir();
    //void setInputSignal(const std::vector<T>& inputSignal);
    //void setInverseFilter(const std::vector<T>& inverseFilter);

    void findIrMaxValue(int& idx, T& value);
    void findRangeByAmplitude(int& min, int& max);

    void reset();

    FrequencyTable<double> _table;
    int _sampleRate = 48000;
    double _fMin = 20.0;
    double _fMax = 20000.0;
    std::vector<T> _inverseFilter;
    std::vector<T> _inputSignal;

    std::vector<T> _ir;
    std::vector<T> _irWin;
    std::vector<T> _winIr;

    std::function<void()> _frChangedCallback = nullptr;
    std::vector<double> _fr;

    int _irMaxValueIndex = 0;
    int _irWindowLeft = 0;
    int _irWindowRight = 0;

    Calibration _calibration = CalibrationNone;
    std::map<Calibration, std::vector<double>> _frCalibrations;
    std::vector<double> _calibratedFr;

    //friend class MeasurementManager;
};

#endif // MEASUREMENTSESSION_H
