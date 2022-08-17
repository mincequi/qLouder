#ifndef RESPONSESIGNAL_H
#define RESPONSESIGNAL_H

#include "Signal.h"

/**
 * @brief The ResponseSignal class
 *
 * Always mono
 */
class ResponseSignal : public Signal
{
public:
    ResponseSignal(AudioBuffer* buffer, int sampleRate);
};

#endif // RESPONSESIGNAL_H
