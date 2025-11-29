#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

#include "actors_base.h"


namespace Services
{
class Valve_rinse : public ActorsBase {
public:
    Valve_rinse(uint8_t pin, unsigned long debounceMs = DEBOUNCE_TIME, unsigned long timeoutMs = 0)
        : ActorsBase(pin, "rinse_valve", debounceMs, timeoutMs) {}
};
} // end of namespace Services