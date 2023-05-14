#pragma once

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
