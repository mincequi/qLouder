#include "Measurement.h"

#include <cstring>
#include <complex>

#include <wavpack/wavpack.h>

#include "Fft.h"
#include "SignalGenerator.h"
#include <status/StatusModel.h>

// Swept Sine Chirps for Measuring Impulse Response
// https://www.thinksrs.com/downloads/pdfs/applicationnotes/SR1_SweptSine.pdf
Measurement::Measurement(int sampleRate,
                         const MonoSignal& ir,
                         const std::map<double, double>& calibration0,
                         const std::map<double, double>& calibration90,
                         Calibration calibration)
    : _sampleRate(sampleRate),
      _ir(ir),
      _calibration(calibration),
      _irWindowLeft(-100 * sampleRate / 1000),
      _irWindowRight(400 * sampleRate / 1000) {
    _frCalibrations[Calibration0] = _table.interpolate(calibration0, false);
    _frCalibrations[Calibration90] = _table.interpolate(calibration90, false);

    // Find max value
    float maxValue = 0.0;
    findIrMaxValue(_ir, _irMaxValueIndex, maxValue);

    // Scale IR
    for (auto& i : _ir) {
        i /= maxValue;
    }

    // Take total of 100ms -> 80ms (or 27.5m)
    // @TODO: think about default right window for bass
    //      220ms(240ms)    ->  4.55Hz (40Hz, 1/12)
    //      300ms(320ms)    ->  3.33Hz (28Hz, 1/12)
    //      400ms(420ms)    ->  2.50Hz (40Hz, 1/24)
    //      460ms(480ms)    ->  2.17Hz (20Hz, 1/12)
    //      480ms(500ms)    ->  2.08Hz (37.5Hz, 1/24)
    //      580ms(600ms)    ->  1.72Hz (31.5Hz, 1/24)

    // Start at -100ms
    setIrWindowLeft(-100.0);
    // End at +400ms
    setIrWindowRight(+400.0);
}

Measurement::Measurement(int sampleRate,
                         const MonoSignal& ir,
                         const std::map<std::string, std::string>& tags)
    : _sampleRate(sampleRate),
      _ir(ir),
      _irWindowLeft(-100 * sampleRate / 1000),
      _irWindowRight(400 * sampleRate / 1000) {

    // Find max value
    float maxValue = 0.0;
    findIrMaxValue(_ir, _irMaxValueIndex, maxValue);

    // Scale IR
    for (auto& i : _ir) {
        i /= maxValue;
    }

    // Start at -100ms
    setIrWindowLeft(-100.0);
    // End at +400ms
    setIrWindowRight(+400.0);
}

int Measurement::sampleRate() const {
    return _sampleRate;
}

const MonoSignal& Measurement::ir() {
    return _ir;
}

const MonoSignal& Measurement::irWindow() {
    if (_irWin.empty()) {
        _irWin.reserve(_irWindowRight - _irWindowLeft + 1);

        const int K1 = std::ceil((_irMaxValueIndex - _irWindowLeft)/4.0);
        double factor = 2.0 * M_PI / (2*K1);
        for (int i = 0; i < K1; ++i) {
            double leftShift = i - K1;
            //_irWin.push_back( { leftShift / sampleRate() * 1000.0, 0.5 * (1.0 - cos(factor * i)) } );
            _irWin.push_back(0.5 * (1.0 - cos(factor * i)));
        }
        for (int i = 0; i < 3*K1; ++i) {
            _irWin.push_back(1.0);
        }

        const int K2 = std::ceil((_irWindowRight - _irMaxValueIndex)/4.0);
        for (int i = 0; i < 3*K2; ++i) {
            _irWin.push_back(1.0);
        }
        factor = 2.0 * M_PI / (2*K2);
        for (int i = 0; i < K2; ++i) {
            //_irWin.push_back( { (double)i / sampleRate() * 1000.0, 0.5 * (1.0 - cos(factor * (i+K2))) } );
            _irWin.push_back(0.5 * (1.0 - cos(factor * (K2+i))));
        }
    }

    return _irWin;
}

const MonoSignal& Measurement::windowedIr() {
    if (_windowedIr.empty()) {
        const auto& ir_ = ir();
        const auto& irWin = irWindow();
        //findRangeByAmplitude(min, max);

        _windowedIr.resize(_irWindowRight - _irWindowLeft);
        for (int i = 0; i < _windowedIr.size(); ++i) {
            _windowedIr.at(i) = ir_.at(i + _irWindowLeft) * irWin.at(i);
        }
        //std::memcpy(_winIr.data(), ir.data() + _irWindowLeft, _winIr.size()*sizeof(float));
    }

    return _windowedIr;
}

void Measurement::setFrChangedCallback(std::function<void()> cb) {
    _frChangedCallback = cb;
}

const std::vector<double>& Measurement::frequencies() {
    return _table.frequencies();
}

const std::vector<double>& Measurement::fr() {
    if (_fr.empty()) {
        const auto& wIr = windowedIr();

        // Extend to whole seconds (with padded zeros)
        int fftLength = wIr.size();
        if (fftLength % _sampleRate)
            fftLength += _sampleRate - (fftLength % _sampleRate);
        MonoSignal buf(fftLength);
        std::memcpy(buf.data(), wIr.data(), wIr.size() * sizeof(float));

        std::vector<std::complex<float>> out(_sampleRate);
        fft<float>(buf, out);

        // Save magnitude
        std::map<double, double> fMap;
        for (int j = 0; j < _sampleRate/2; ++j) {
            fMap[(double)j] = 20 * log10(abs(out[j]));
        }

        _fr = _table.interpolate(fMap, true);
    }

    return _fr;
}

void Measurement::setCalibration(Calibration calibration) {
    if (_calibration == calibration) return;

    _calibration = calibration;
    _calibratedFr.clear();

    if (_frChangedCallback) {
        _frChangedCallback();
    }
}

const std::vector<double>& Measurement::calibratedFr() {
    const auto& fr_ = fr();
    if (_calibratedFr.empty()) {
        if (_frCalibrations[_calibration].empty()) {
            _calibratedFr = fr();
        } else {
            for (int i = 0; i < fr_.size(); ++i) {
                _calibratedFr.push_back(fr_.at(i) + _frCalibrations[_calibration].at(i));
            }
        }
    }

    return _calibratedFr;
}

const std::vector<double>& Measurement::frCalibration(Calibration calibration) {
    return _frCalibrations[calibration];
}

double Measurement::irWindowLeft() const {
    return (double)(_irMaxValueIndex - _irWindowLeft)/_sampleRate * -1000.0;
}

void Measurement::setIrWindowLeft(double ms) {
    _irWindowLeft = _irMaxValueIndex + _sampleRate * ms / 1000.0;
    _irWindowLeft = std::max(_irWindowLeft, 0);

    reset();
}

double Measurement::irWindowRight() const {
    return (double)(_irWindowRight - _irMaxValueIndex)/_sampleRate * 1000.0;
}

void Measurement::setIrWindowRight(double ms) {
    _irWindowRight = _irMaxValueIndex + _sampleRate * ms / 1000.0;
    _irWindowRight = std::min(_irWindowRight, (int)_ir.size()-1);

    reset();
}

void Measurement::findIrMaxValue(const MonoSignal& ir, int& idx, float& value) {
    for (int i = 0; i < ir.size(); ++i) {
        if (value < fabs(ir.at(i))) {
            value = fabs(ir.at(i));
            idx = i;
        }
    }
}

void Measurement::findRangeByAmplitude(int& min, int& max) {
    float ampMax = 0.0f;
    for (int i = min; i >= 0; --i) {
        // 0.00025  -> 72dB
        // 0.000063 -> 84dB
        // 0.000016 -> 96dB
        if (fabs(ir().at(i)) > 0.00025f) {
            min = i;
            break;
        }
    }

    for (int i = ir().size() - 1; i >= 0; --i) {
        if (fabs(ir().at(i)) > 0.00025f) {
            max = i;
            break;
        }
    }
}

void Measurement::reset() {
    _irWin.clear();
    _windowedIr.clear();
    _fr.clear();
    _calibratedFr.clear();

    if (_frChangedCallback) {
        _frChangedCallback();
    }
}
