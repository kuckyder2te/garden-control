#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

#include "actors_base.h"


namespace Services
{
class Pool_fill : public ActorsBase {
public:
    Pool_fill(uint8_t pin, unsigned long debounceMs = DEBOUNCE_TIME, unsigned long timeoutMs = 0)
        : ActorsBase(pin, "pool/fill", timeoutMs) {}
};
} // end of namespace Services