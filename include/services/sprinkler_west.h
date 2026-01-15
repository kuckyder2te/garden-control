#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

#include "actors_base.h"


namespace Services
{
class Sprinkler_west : public ActorsBase {
public:
    Sprinkler_west(uint8_t pin, unsigned long debounceMs = DEBOUNCE_TIME, unsigned long timeoutMs = 0)
        : ActorsBase(pin, "sprinkler/west", timeoutMs) {}
};
} // end of namespace Services