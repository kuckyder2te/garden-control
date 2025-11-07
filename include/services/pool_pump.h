#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond
#include "valve_base.h"

namespace Services {
class Pool_pump : public ValveBase {
public:
    Pool_pump(uint8_t pin, unsigned long debounceMs = 200, unsigned long timeoutMs = 0)
        : ValveBase(pin, "pool_pump", debounceMs, timeoutMs) {}
};
} // namespace Services
