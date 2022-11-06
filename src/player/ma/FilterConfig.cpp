#include "FilterConfig.h"

namespace ma {

FilterConfig::FilterConfig() {
}

FilterConfig::FilterConfig(Type type, float freq, float q, float gain, StandardSampleRate sampleRate) :
    _type(type), _freq(freq), _q(q), _gain(gain), _sampleRate(sampleRate) {
}

FilterConfig& FilterConfig::operator=(const FilterConfig& other) {
    if (_type != other._type
            || _freq != other._freq
            || _q != other._q
            || _gain != other._gain
            || _sampleRate != other._sampleRate) {
        _type = other._type;
        _freq = other._freq;
        _q = other._q;
        _gain = other._gain;
        _sampleRate = other._sampleRate;

        _isDirty = true;
    }

    return *this;
}

} // namespace ma
