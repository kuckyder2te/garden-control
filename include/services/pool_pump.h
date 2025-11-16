#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

#include "actors_base.h"

namespace Services {
class Pool_pump : public ActorsBase {
public:
    Pool_pump(uint8_t pin, unsigned long debounceMs = 200, unsigned long timeoutMs = 0)
        : ActorsBase(pin, "pool_pump", debounceMs, timeoutMs) {}
};
} // namespace Services
