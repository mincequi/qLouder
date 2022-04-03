#include "AudioFilter.h"

#include <cmath>
#include <numbers>

AudioFilter::AudioFilter(FilterType type, double f, double g, double q) :
    _type(type),
    _f(f),
    _g(g),
    _q(q) {
    switch (type) {
    case FilterType::Peak: {
        double A = pow(10, _g/40.0);
        double w0 = 2.0 * std::numbers::pi * _f /48000.0;
        double alpha = sin(w0) * 0.5 / _q;

        double alpha1 = alpha*A;
        double alpha2 = alpha/A;
        double a0     = 1.0 + alpha2;

        _biquad.b0 = ( 1.0 + alpha1 ) / a0;
        _biquad.b1 = (-2.0 * cos(w0)) / a0;
        _biquad.b2 = ( 1.0 - alpha1 ) / a0;
        _biquad.a1 = _biquad.b1;
        _biquad.a2 = ( 1.0 - alpha2 ) / a0;
        break;
    }
    case FilterType::LowPass: {
        double w0 = 2*std::numbers::pi * _f / 48000.0;
        double alpha = sin(w0)*0.5/_q;
        double a0    = 1.0 + alpha;

        _biquad.b1 = ( 1.0 - cos(w0) ) / a0;
        _biquad.b0 = _biquad.b1 * 0.5;
        _biquad.b2 = _biquad.b0;
        _biquad.a1 = (-2.0 * cos(w0)) / a0;
        _biquad.a2 = ( 1.0 - alpha  ) / a0;
        break;
    }
    case FilterType::HighPass: {
        double w0 = 2.0 * std::numbers::pi * _f / 48000.0;
        double alpha = sin(w0)*0.5/_q;
        double a0    = 1.0 + alpha;

        _biquad.b1 = -( 1.0 + cos(w0) ) / a0;
        _biquad.b0 = _biquad.b1 * -0.5;
        _biquad.b2 = _biquad.b0;
        _biquad.a1 = (-2.0 * cos(w0)) / a0;
        _biquad.a2 = ( 1.0 - alpha  ) / a0;
        break;
    }
    case FilterType::LowShelf: {
        double A = pow(10, _g/40.0);
        double w0 = 2.0 * std::numbers::pi * _f / 48000.0;
        double cos_w0 = cos(w0);
        double alpha = sin(w0)*0.5/_q;
        double alpha2 = 2*sqrt(A)*alpha;
        double a0 = (A+1) + (A-1)*cos_w0 + alpha2;

        _biquad.b0 =  (  A*( (A+1) - (A-1)*cos_w0 + alpha2) )/a0;
        _biquad.b1 =  (2*A*( (A-1) - (A+1)*cos_w0         ) )/a0;
        _biquad.b2 =  (  A*( (A+1) - (A-1)*cos_w0 - alpha2) )/a0;
        _biquad.a1 =  ( -2*( (A-1) + (A+1)*cos_w0         ) )/a0;
        _biquad.a2 =  (      (A+1) + (A-1)*cos_w0 - alpha2  )/a0;
        break;
    }
    case FilterType::HighShelf: {
        double A = pow(10, _g/40.0);
        double w0 = 2.0 * std::numbers::pi * _f / 48000.0;
        double cos_w0 = cos(w0);
        double alpha = sin(w0)*0.5/_q;
        double alpha2 = 2*sqrt(A)*alpha;
        double a0 = (A+1) - (A-1)*cos_w0 + alpha2;

        _biquad.b0 =  (  A*( (A+1) + (A-1)*cos_w0 + alpha2) )/a0;
        _biquad.b1 = (-2*A*( (A-1) + (A+1)*cos_w0         ) )/a0;
        _biquad.b2 =  (  A*( (A+1) + (A-1)*cos_w0 - alpha2) )/a0;
        _biquad.a1 =  (  2*( (A-1) - (A+1)*cos_w0         ) )/a0;
        _biquad.a2 =  (      (A+1) - (A-1)*cos_w0 - alpha2  )/a0;
        break;
    }
    case FilterType::AllPass: {
        double w0 = 2.0 * std::numbers::pi * _f / 48000.0;
        double cos_w0 = cos(w0);
        double alpha = sin(w0)*0.5/_q;
        double a0 = 1 + alpha;

        _biquad.b0 = (1 - alpha)/a0;
        _biquad.b1 = (-2*cos_w0)/a0;
        _biquad.b2 = 1.0;
        _biquad.a1 = _biquad.b1;
        _biquad.a2 = _biquad.b0;
        break;
    }

    case FilterType::Invalid:
        break;

    case FilterType::Loudness:
        // Filter 1> t: pk, f: 35.5,  q: 0.537, g: <phon>/40 * 12db
        // Filter 2> t: pk, f: 100,   q: 0.25,  g: <phon>/40 * 9db
        // Filter 3> t: hs, f: 10000, q: 0.78,  g: <phon>/40 * 9db
        _filters.push_back(AudioFilter(FilterType::Peak, 35.5, g * 0.3, 0.537));
        _filters.push_back(AudioFilter(FilterType::Peak, 100.0, g * 0.225, 0.25));
        _filters.push_back(AudioFilter(FilterType::HighShelf, 10000.0, g * 0.225, 0.78));
        break;
    }
}

std::vector<std::complex<double>> AudioFilter::response(const std::vector<double>& fs, int cascades) const {
    std::vector<std::complex<double>> out;
    out.reserve(fs.size());

    if (!_filters.empty()) {
        //out.resize(fs.size());
        for (const auto& f : _filters) {
            if (out.empty()) {
                out = f.response(fs, 1);
            } else {
                auto res = f.response(fs, 1);
                for (int i = 0; i < fs.size(); ++i) {
                    out.at(i) *= res.at(i);
                }
            }
        }
        return out;
    }

    for (int i = 0; i < fs.size(); ++i) {
        double w = 2.0 * std::numbers::pi * fs.at(i)/48000;
        std::complex<double> z(cos(w), sin(w));
        std::complex<double> numerator = _biquad.b0 + (_biquad.b1 + _biquad.b2*z)*z;
        std::complex<double> denominator = 1.0 + (_biquad.a1 + _biquad.a2*z)*z;
        std::complex<double> res = numerator / denominator;
        if (cascades != 1) {
            res = pow(res, cascades);
        }

        out.push_back(res);
    }
    return out;
}
