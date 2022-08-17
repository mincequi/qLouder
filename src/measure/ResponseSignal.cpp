#include "ResponseSignal.h"

#include "AudioBuffer.h"

ResponseSignal::ResponseSignal(AudioBuffer* buffer, int sampleRate) :
    Signal(buffer) {
    _sampleRate = sampleRate;
}
