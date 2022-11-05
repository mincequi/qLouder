#include "FilterNode.h"

#include <cmath>

#include "NodeGraph.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

namespace ma {

FilterNode::FilterNode(const NodeGraph& nodeGraph,
                       const FilterConfig& config)
    : NodeTemplate(this->baseNode) {
    _config = config;
    const auto config_ = ma_biquad_node_config_init(nodeGraph._nodeGraphBase.base.pOutputBuses->channels, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    //const auto config_ = ma_biquad_node_config_init(2, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    //ma_biquad_node_init(const_cast<NodeGraph*>(&nodeGraph), &config_, NULL, &_biquad);
    ma_biquad_node_init(const_cast<ma_node_graph*>(&nodeGraph._nodeGraphBase), &config_, NULL, this);
    initBiquad();
}

FilterNode::~FilterNode() {
    //ma_biquad_node_uninit(&_biquad, NULL);
    ma_biquad_node_uninit(this, NULL);
}

FilterNode& FilterNode::operator=(const FilterConfig& config) {
    _config = config;
    if (_config._isDirty) {
        initBiquad();
    }

    return *this;
}

const FilterConfig& FilterNode::operator=(const FilterNode& node) {
    return _config;
}

void FilterNode::initBiquad() {
    switch (_config._type) {
    case FilterConfig::Type::None:
        this->biquad.b0.f32 = 1.0;
        this->biquad.b1.f32 = 0.0;
        this->biquad.b2.f32 = 0.0;
        this->biquad.a1.f32 = 0.0;
        this->biquad.a2.f32 = 0.0;
        break;
    case FilterConfig::Type::Peak: {
        const double A = pow(10, _config._gain/40.0);
        const double w0 = 2.0 * M_PI * _config._freq / (double)_config._sampleRate;
        const double alpha = sin(w0) * 0.5 / _config._q;

        const double alpha1 = alpha*A;
        const double alpha2 = alpha/A;
        const double a0     = 1.0 + alpha2;

        this->biquad.b0.f32 = ( 1.0 + alpha1 ) / a0;
        this->biquad.b1.f32 = (-2.0 * cos(w0)) / a0;
        this->biquad.b2.f32 = ( 1.0 - alpha1 ) / a0;
        this->biquad.a1.f32 = this->biquad.b1.f32;
        this->biquad.a2.f32 = ( 1.0 - alpha2 ) / a0;
        break;
    }
    case FilterConfig::Type::LowPass: {
        const double w0 = 2*M_PI * _config._freq / (double)_config._sampleRate;
        const double alpha = sin(w0)*0.5/_config._q;
        const double a0    = 1.0 + alpha;

        this->biquad.b1.f32 = (const float)(( 1.0 - cos(w0) ) / a0);
        this->biquad.b0.f32 = this->biquad.b1.f32 * 0.5;
        this->biquad.b2.f32 = this->biquad.b0.f32;
        this->biquad.a1.f32 = (-2.0 * cos(w0)) / a0;
        this->biquad.a2.f32 = ( 1.0 - alpha  ) / a0;
        break;
    }
    case FilterConfig::Type::HighPass: {
        const double w0 = 2.0 * M_PI * _config._freq / (double)_config._sampleRate;
        const double alpha = sin(w0)*0.5/_config._q;
        const double a0    = 1.0 + alpha;

        this->biquad.b1.f32 = -( 1.0 + cos(w0) ) / a0;
        this->biquad.b0.f32 = this->biquad.b1.f32 * -0.5;
        this->biquad.b2.f32 = this->biquad.b0.f32;
        this->biquad.a1.f32 = (-2.0 * cos(w0)) / a0;
        this->biquad.a2.f32 = ( 1.0 - alpha  ) / a0;
        break;
    }
    case FilterConfig::Type::LowShelf: {
        const double A = pow(10, _config._gain/40.0);
        const double w0 = 2.0 * M_PI * _config._freq / (double)_config._sampleRate;
        const double cos_w0 = cos(w0);
        const double alpha = sin(w0)*0.5/_config._q;
        const double alpha2 = 2*sqrt(A)*alpha;
        const double a0 = (A+1) + (A-1)*cos_w0 + alpha2;

        this->biquad.b0.f32 =  (  A*( (A+1) - (A-1)*cos_w0 + alpha2) )/a0;
        this->biquad.b1.f32 =  (2*A*( (A-1) - (A+1)*cos_w0         ) )/a0;
        this->biquad.b2.f32 =  (  A*( (A+1) - (A-1)*cos_w0 - alpha2) )/a0;
        this->biquad.a1.f32 =  ( -2*( (A-1) + (A+1)*cos_w0         ) )/a0;
        this->biquad.a2.f32 =  (      (A+1) + (A-1)*cos_w0 - alpha2  )/a0;
        break;
    }
    case FilterConfig::Type::HighShelf: {
        const double A = pow(10, _config._gain/40.0);
        const double w0 = 2.0 * M_PI * _config._freq / (double)_config._sampleRate;
        const double cos_w0 = cos(w0);
        const double alpha = sin(w0)*0.5/_config._q;
        const double alpha2 = 2*sqrt(A)*alpha;
        const double a0 = (A+1) - (A-1)*cos_w0 + alpha2;

        this->biquad.b0.f32 =  (  A*( (A+1) + (A-1)*cos_w0 + alpha2) )/a0;
        this->biquad.b1.f32 = (-2*A*( (A-1) + (A+1)*cos_w0         ) )/a0;
        this->biquad.b2.f32 =  (  A*( (A+1) + (A-1)*cos_w0 - alpha2) )/a0;
        this->biquad.a1.f32 =  (  2*( (A-1) - (A+1)*cos_w0         ) )/a0;
        this->biquad.a2.f32 =  (      (A+1) - (A-1)*cos_w0 - alpha2  )/a0;
        break;
    }
    }
    _config._isDirty = false;
}

} // namespace ma
