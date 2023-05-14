#pragma once

#include <functional>

#include "ImpulseResponse.h"
#include "Types.h"
#include <common/FrequencyTable.h>

/**
 * @brief The Measurement class
 *
 * This is a passive data class to hold any values to save and restore a
 * measurement from disk. It shall also encapsulate "basic" and "raw" data
 * operation/manipulation like windowing and FFT.
 * It shall not hold state.
 */
class Measurement {
public:
    explicit Measurement(int sampleRate,
                         // TODO: change inputSignal and inverseFilter to ir
                         const MonoSignal& ir,
                         const std::map<double, double>& calibration0 = {},
                         const std::map<double, double>& calibration90 = {},
                         Calibration calibration = CalibrationNone);

    Measurement(int sampleRate,
                const MonoSignal& ir,
                const std::map<std::string, std::string>& tags);

    int sampleRate() const;
    const MonoSignal& ir();

    const std::string& speaker() const;
    const std::string& microphone() const;
    const std::vector<double>& calibration(Calibration calibration) const;
    void setCalibration(Calibration calibration);

    uint32_t recordDate() const;

    // TODO: candidate to be moved
    const MonoSignal& irWindow();
    const MonoSignal& windowedIr();

    double irWindowLeft() const;
    void setIrWindowLeft(double ms);

    double irWindowRight() const;
    void setIrWindowRight(double ms);

    // Frequency response related methods
    // TODO: remove this one
    const std::vector<double>& frequencies();
    void setFrChangedCallback(std::function<void()>);

    const std::vector<double>& calibratedFr();
    const std::vector<double>& frCalibration(Calibration calibration);

private:
    enum class qLouderTagKey {
        Version,

        Date,
        Name,
        Description,

        MicrophoneName,
        MicrophoneCalibration,
        MicrophoneCalibration0,
        MicrophoneCalibration90,

        ImpulseResponseWindowLeft,
        ImpulseResponseWindowRight
    };

    const std::vector<double>& fr();

    static void findIrMaxValue(const MonoSignal& ir, int& idx, float& value);
    void findRangeByAmplitude(int& min, int& max);

    void reset();

    FrequencyTable<double> _table;
    int _sampleRate = 48000;

    MonoSignal _ir;
    MonoSignal _irWin;
    MonoSignal _windowedIr;

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
