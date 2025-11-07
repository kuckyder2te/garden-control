#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

#include "valve_base.h"


namespace Services
{
class Valve_rinse : public ValveBase {
public:
    Valve_rinse(uint8_t pin, unsigned long debounceMs = 200, unsigned long timeoutMs = 0)
        : ValveBase(pin, "rinse_valve", debounceMs, timeoutMs) {}
};
} // namespace Services