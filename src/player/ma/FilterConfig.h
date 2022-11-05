#pragma once

#include "Types.h"

namespace ma {

class FilterConfig {
public:
    enum class Type {
        None,
        Peak,
        LowPass,
        HighPass,
        LowShelf,
        HighShelf
    };

    FilterConfig();
    explicit FilterConfig(Type type,
                          float freq,
                          float q,
                          float gain = 0.0,
                          StandardSampleRate sampleRate = StandardSampleRate::Rate48000);
    FilterConfig& operator=(const FilterConfig& other);

private:
    bool _isDirty = true;
    Type _type = Type::None;
    float _freq = 0.0f;
    float _q = 0.0f;
    float _gain = 0.0f;
    StandardSampleRate _sampleRate = StandardSampleRate::Rate48000;

    friend class FilterNode;
};

} // namespace ma
