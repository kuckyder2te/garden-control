#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

#include "actors_base.h"

namespace Services
{
class Sprinkler_east : public ActorsBase {
public:
    Sprinkler_east(uint8_t pin, unsigned long timeoutMs = 0)
        : ActorsBase(pin, "sprinkler/east", timeoutMs) {}
};
} // end of namespace Services