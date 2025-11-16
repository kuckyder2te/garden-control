#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

#include "actors_base.h"

namespace Services
{
class Valve_garden : public ActorsBase {
public:
    Valve_garden(uint8_t pin, unsigned long debounceMs = 200, unsigned long timeoutMs = 0)
        : ActorsBase(pin, "garden_valve", debounceMs, timeoutMs) {}
};
} // namespace Services