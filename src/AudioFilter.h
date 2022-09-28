#pragma once

#include <complex>
#include <vector>

enum class FilterType : int8_t {
    Invalid     = 0,

    // Linear filters
    Peak        = 1,
    LowPass     = 2,
    HighPass    = 3,
    LowShelf    = 4,
    HighShelf   = 5,
    AllPass,

    // Special filters
    Loudness = 64
};

class AudioFilter {
public:
    AudioFilter(FilterType type = FilterType::Invalid,
           double f = 0.0,
           double g = 0.0,
           double q = 0.0);

    std::vector<std::complex<double>> response(const std::vector<double>& fs, int cascades) const;

private:
    struct BiQuad {
        double b0 = 0.0, b1 = 0.0, b2 = 0.0;
        double a1 = 0.0, a2 = 0.0;
    };

    FilterType _type = FilterType::Invalid;
    double _f = 0.0;
    double _g = 0.0;
    double _q = 0.0;
    BiQuad _biquad;
    std::vector<AudioFilter> _filters;
};
