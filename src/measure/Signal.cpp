#include "Signal.h"

#include <CDSPResampler.h>

#include "AudioBuffer.h"

Signal::Channels Signal::channels() const {
    return _channels;
}

void Signal::setChannels(Channels channels) {
    _channels = channels;
}

int Signal::sampleRate() const {
    return _sampleRate;
}

void Signal::resample(int newSampleRate) {
    if (newSampleRate == _sampleRate) return;

    std::vector<float> newData((double)_buffer->data.size() * newSampleRate / _sampleRate);
    auto resampler = r8b::CDSPResampler(_sampleRate, newSampleRate, _buffer->data.size());
    resampler.oneshot(_buffer->data.data(), _buffer->data.size(), newData.data(), newData.size());

    _buffer->data = newData;
    _sampleRate = newSampleRate;
}

Signal::Signal(AudioBuffer* buffer)
    : _buffer(buffer) {
}

const std::vector<float>& Signal::data() const {
    return _buffer->data;
}
