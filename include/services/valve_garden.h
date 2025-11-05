#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

// #include "../message.h"
// #include "../messageBroker.h"
#include "valve_base.h"


namespace Services
{
class Valve_garden : public ValveBase {
public:
    Valve_garden(uint8_t pin, unsigned long debounceMs = 200, unsigned long timeoutMs = 0)
        : ValveBase(pin, "Garden valve", debounceMs, timeoutMs) {}
};
} // namespace Services