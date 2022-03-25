#include "Measurement.h"

#include <cstring>
#include <complex>

#include "Fft.h"
#include "SignalGenerator.h"
#include <FrequencyTable.h>
#include <status/StatusModel.h>

// Swept Sine Chirps for Measuring Impulse Response
// https://www.thinksrs.com/downloads/pdfs/applicationnotes/SR1_SweptSine.pdf

template <class T>
Measurement<T>::Measurement(int sampleRate,
                            double fMin,
                            double fMax,
                            const std::vector<T>& inputSignal,
                            const std::vector<T>& inverseFilter,
                            const std::map<double, double>& calibration0,
                            const std::map<double, double>& calibration90,
                            Calibration calibration)
    : _sampleRate(sampleRate),
      _fMin(fMin),
      _fMax(fMax),
      _inputSignal(inverseFilter.size()*2),
      _inverseFilter(inverseFilter.size()*2),
      _calibration(calibration),
      _table(24, fMin, fMax),
      _irWindowLeft(-100 * sampleRate / 1000),
      _irWindowRight(400 * sampleRate / 1000) {
    _frCalibrations[Calibration0] = _table.interpolate(calibration0, false);
    _frCalibrations[Calibration90] = _table.interpolate(calibration90, false);
    std::memcpy(_inputSignal.data(), inputSignal.data(), inputSignal.size()*sizeof(T));
    std::memcpy(_inverseFilter.data(), inverseFilter.data(), inverseFilter.size()*sizeof(T));
}

template <class T>
const std::vector<T>& Measurement<T>::ir() {
    if (_ir.empty()) {
        // compute deconvolution of the system’s impulse response
        const auto fftLength = _inputSignal.size();

        // FFT measured signal
        std::vector<std::complex<T>> fftInputSignal(fftLength);
        fft<T>(_inputSignal, fftInputSignal);

        // FFT reverse filter
        std::vector<std::complex<T>> fftInverseSignal(fftLength);
        fft<T>(_inverseFilter, fftInverseSignal);

        // Compute product
        std::vector<std::complex<T>> product(fftLength);
        // @TODO: do we need this scaling?
        const T normFactor = 1.0 / fftLength;
        for (size_t i = 0; i < fftLength; i++) {
            product[i] = fftInputSignal[i] * fftInverseSignal[i] * normFactor;
        }

        // Compute impulse response
        _ir.resize(fftLength);
        ifft<T>(product, _ir);

        // Find max value
        T maxValue = 0.0;
        findIrMaxValue(_irMaxValueIndex, maxValue);

        // Scale IR
        // @TODO: here we apply the second scaling. So, is the first one actually needed?
        for (auto& i : _ir) {
            i /= maxValue;
        }

        // Start at -200ms
        setIrWindowLeft(-100.0);
        // End at +800ms
        setIrWindowRight(+400.0);

        // Take total of 100ms -> 80ms (or 27.5m)
        // @TODO: think about default right window for bass
        //      220ms(240ms)    ->  4.55Hz (40Hz, 1/12)
        //      300ms(320ms)    ->  3.33Hz (28Hz, 1/12)
        //      400ms(420ms)    ->  2.50Hz (40Hz, 1/24)
        //      460ms(480ms)    ->  2.17Hz (20Hz, 1/12)
        //      480ms(500ms)    ->  2.08Hz (37.5Hz, 1/24)
        //      580ms(600ms)    ->  1.72Hz (31.5Hz, 1/24)
    }

    return _ir;
}

template <class T>
const std::vector<T>& Measurement<T>::irWindow() {
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

template <class T>
const std::vector<T>& Measurement<T>::windowedIr() {
    if (_winIr.empty()) {
        const auto& ir_ = ir();
        const auto& irWin = irWindow();
        //findRangeByAmplitude(min, max);

        _winIr.resize(_irWindowRight - _irWindowLeft);
        for (int i = 0; i < _winIr.size(); ++i) {
            _winIr.at(i) = ir_.at(i + _irWindowLeft) * irWin.at(i);
        }
        //std::memcpy(_winIr.data(), ir.data() + _irWindowLeft, _winIr.size()*sizeof(T));
    }

    return _winIr;
}

template <class T>
void Measurement<T>::setFrChangedCallback(std::function<void()> cb) {
    _frChangedCallback = cb;
}

template <class T>
const std::vector<T>& Measurement<T>::frequencies() {
    return _table.frequencies();
}

template <class T>
const std::vector<T>& Measurement<T>::fr() {
    if (_fr.empty()) {
        const auto& wIr = windowedIr();

        // Extend to hole seconds (with padded zeros)
        int fftLength = wIr.size();
        if (fftLength % _sampleRate)
            fftLength += _sampleRate - (fftLength % _sampleRate);
        std::vector<T> buf(fftLength);
        std::memcpy(buf.data(), wIr.data(), wIr.size() * sizeof(T));

        std::vector<std::complex<T>> out(_sampleRate);
        fft<T>(buf, out);

        // Save magnitude
        std::map<double, double> fMap;
        for (int j = 0; j < _sampleRate/2; ++j) {
            fMap[(double)j] = 20 * log10(abs(out[j]));
        }

        _fr = _table.interpolate(fMap, true);
    }

    return _fr;
}

template <class T>
void Measurement<T>::setFrCalibration(Calibration calibration) {
    if (_calibration == calibration) return;

    _calibration = calibration;
    _calibratedFr.clear();

    if (_frChangedCallback) {
        _frChangedCallback();
    }
}

template <class T>
const std::vector<T>& Measurement<T>::calibratedFr() {
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

template <class T>
const std::vector<T>& Measurement<T>::frCalibration(Calibration calibration) {
    return _frCalibrations[calibration];
}

template <class T>
double Measurement<T>::irWindowLeft() const {
    return (double)(_irMaxValueIndex - _irWindowLeft)/_sampleRate * -1000.0;
}

template <class T>
void Measurement<T>::setIrWindowLeft(double ms) {
    _irWindowLeft = _irMaxValueIndex + _sampleRate * ms / 1000.0;
    _irWindowLeft = std::max(_irWindowLeft, 0);

    reset();
}

template <class T>
double Measurement<T>::irWindowRight() const {
    return (double)(_irWindowRight - _irMaxValueIndex)/_sampleRate * 1000.0;
}

template <class T>
void Measurement<T>::setIrWindowRight(double ms) {
    _irWindowRight = _irMaxValueIndex + _sampleRate * ms / 1000.0;
    _irWindowRight = std::min(_irWindowRight, (int)_ir.size()-1);

    reset();
}

template <class T>
void Measurement<T>::findIrMaxValue(int& idx, T& value) {
    for (int i = 0; i < _ir.size(); ++i) {
        if (value < fabs(_ir.at(i))) {
            value = fabs(_ir.at(i));
            idx = i;
        }
    }
}

template <class T>
void Measurement<T>::findRangeByAmplitude(int& min, int& max) {
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

template <class T>
void Measurement<T>::reset() {
    _irWin.clear();
    _winIr.clear();
    _fr.clear();
    _calibratedFr.clear();

    if (_frChangedCallback) {
        _frChangedCallback();
    }
}

template class Measurement<float>;
template class Measurement<double>;
