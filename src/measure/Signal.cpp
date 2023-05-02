#include "Signal.h"

#include <CDSPResampler.h>

#include "AudioBuffer.h"

Signal::Signal(AudioBuffer* buffer) :
    _buffer(buffer) {
}

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

    /*
    Buffer oldData(_buffer->data.size());
    std::memcpy(oldData.getData(), _buffer->data.data(), _buffer->data.size() * 4);
    Buffer newData((double)_buffer->data.size() * newSampleRate / _sampleRate);

    dsp::ConverterImplR8brain resampler(_sampleRate, newSampleRate, 1, 1, _buffer->data.size());
    resampler.convert(&oldData, &newData);
    */

    std::vector<float> newData((double)_buffer->data.size() * newSampleRate / _sampleRate);
    auto resampler = r8b::CDSPResampler24(_sampleRate, newSampleRate, _buffer->data.size());
    resampler.oneshot(_buffer->data.data(), _buffer->data.size(), newData.data(), newData.size());

    _buffer->data = std::move(newData);
    _sampleRate = newSampleRate;
}

const std::vector<float>& Signal::data() const {
    return _buffer->data;
}
