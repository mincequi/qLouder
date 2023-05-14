#include "Farina.h"

#include <complex>

#include "Fft.h"
#include "InverseSignal.h"
#include "SignalGenerator.h"

Farina::Farina(AudioBuffer* outputBuffer) :
    _outputBuffer(outputBuffer),
    _excitationSignal(_outputBuffer) {
}

const ExcitationSignal& Farina::excitationSignal() {

    if (_excitationSignal.samplesPerOctave() != _durationPerOctave / 1000.0f * _sampleRate ||
            _excitationSignal.channels() != _channels ||
            _excitationSignal.volume() != _level ||
            _excitationSignal.minF() != _beginFrequency ||
            _excitationSignal.maxF() != _endFrequency) {

        // "Optimizing the exponential sine sweep (ESS) signal for in situ measurements on noise barriers"
        // https://www.conforg.fr/euronoise2015/proceedings/data/articles/000420.pdf
        // Fade-In: 1 octave, Fade-Out: 1/24th octave
        _excitationSignal = SignalFactory::createSineSweep(_outputBuffer,
                                                           _channels,
                                                           _sampleRate,
                                                           _beginFrequency,
                                                           _endFrequency,
                                                           _durationPerOctave / 1000.0f * _sampleRate,
                                                           _sampleRate,
                                                           _sampleRate);
        _excitationSignal.fadeIn(_durationPerOctave * _sampleRate / 1000);
        _excitationSignal.fadeOut(_durationPerOctave * _sampleRate / 24000);

        _inverseSignal = InverseSignal::from(_excitationSignal);

        _excitationSignal.setVolume(_level);
    }

    return _excitationSignal;
}

const ImpulseResponse& Farina::impulseResponse(const MonoSignal& measuredSignal) {
    const size_t fftLength = std::max(measuredSignal.size(), _inverseSignal.data().size())*2;
    MonoSignal measuredSignal_(fftLength);
    MonoSignal inverseSignal_(fftLength);
    std::memcpy(measuredSignal_.data(), measuredSignal.data(), measuredSignal.size()*sizeof(float));
    std::memcpy(inverseSignal_.data(), _inverseSignal.data().data(), _inverseSignal.data().size()*sizeof(float));

    // compute deconvolution of the system’s impulse response

    // FFT measured signal
    std::vector<std::complex<float>> fftInputSignal(fftLength);
    fft<float>(measuredSignal_, fftInputSignal);

    // FFT reverse filter
    std::vector<std::complex<float>> fftInverseSignal(fftLength);
    fft<float>(inverseSignal_, fftInverseSignal);

    // Compute product
    std::vector<std::complex<float>> product(fftLength);
    // @TODO: do we need this scaling?
    //const float normFactor = 1.0 / fftLength;
    for (size_t i = 0; i < fftLength; i++) {
        product[i] = fftInputSignal[i] * fftInverseSignal[i]; // * normFactor;
    }

    // Compute impulse response
    _ir.resize(fftLength);
    ifft<float>(product, _ir);

    return _ir;
}

void Farina::setChannels(Signal::Channels channels) {
    if (channels == _channels) return;

    _channels = channels;
    _isDirty = true;
}

void Farina::setSampleRate(int rate) {
    if (rate == _sampleRate) return;

    _sampleRate = rate;
    _isDirty = true;
}

void Farina::setBeginFrequency(double f) {
    if (f == _beginFrequency) return;

    _beginFrequency = f;
    _isDirty = true;
}

void Farina::setEndFrequency(double f) {
    if (f == _endFrequency) return;

    _endFrequency = f;
    _isDirty = true;
}

void Farina::setDurationPerOctave(int ms)  {
    if (ms == _durationPerOctave) return;

    _durationPerOctave = ms;
    _isDirty = true;
}

void Farina::setLevel(int dB)  {
    if (dB == _level) return;

    _level = dB;
    _isDirty = true;
}

MonoSignal Farina::computeIr(const MonoSignal& measuredSignal, const MonoSignal& inverseSignal) {
    const size_t fftLength = std::max(measuredSignal.size(), inverseSignal.size())*2;
    MonoSignal measuredSignal_(fftLength);
    MonoSignal inverseSignal_(fftLength);
    std::memcpy(measuredSignal_.data(), measuredSignal.data(), measuredSignal.size()*sizeof(float));
    std::memcpy(inverseSignal_.data(), inverseSignal.data(), inverseSignal.size()*sizeof(float));

    // compute deconvolution of the system’s impulse response

    // FFT measured signal
    std::vector<std::complex<float>> fftInputSignal(fftLength);
    fft<float>(measuredSignal_, fftInputSignal);

    // FFT reverse filter
    std::vector<std::complex<float>> fftInverseSignal(fftLength);
    fft<float>(inverseSignal_, fftInverseSignal);

    // Compute product
    std::vector<std::complex<float>> product(fftLength);
    // @TODO: do we need this scaling?
    //const float normFactor = 1.0 / fftLength;
    for (size_t i = 0; i < fftLength; i++) {
        product[i] = fftInputSignal[i] * fftInverseSignal[i]; // * normFactor;
    }

    // Compute impulse response
    MonoSignal ir;
    ir.resize(fftLength);
    ifft<float>(product, ir);

    return ir;
}
