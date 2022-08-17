#include "InverseFilter.h"

#include <math.h>

#include "ExcitationSignal.h"

InverseFilter InverseFilter::from(const ExcitationSignal& excitationSignal) {
    InverseFilter filter;

    const auto outSize = excitationSignal.data().size() / 2;
    filter._data.resize(outSize);
    const size_t channelOffset = excitationSignal.channels() == Signal::Channels::Right ? 1 : 0;
    for (size_t i = 0; i < outSize; ++i) {
        filter._data[outSize-1-i] = excitationSignal.data()[i*2 + channelOffset];
    }

    filter.amplitudeEnvelope(excitationSignal.samplesOffsetFront(),
                             excitationSignal.samplesOffsetBack(),
                             excitationSignal.minF(),
                             excitationSignal.maxF());

    return filter;
}

InverseFilter::InverseFilter() {
}

const std::vector<float>& InverseFilter::data() const {
    return _data;
}

void InverseFilter::amplitudeEnvelope(int offsetFront,
                                      int offsetBack,
                                      double minF,
                                      double maxF) {
    const int signalLength = _data.size() - offsetFront - offsetBack;
    const double factor = log2(maxF/minF)/signalLength;

    auto it = _data.begin() + offsetFront;
    for (int i = 0; i < signalLength; ++i) {
        *it++ *= pow(0.5, factor * i);
    }
}
