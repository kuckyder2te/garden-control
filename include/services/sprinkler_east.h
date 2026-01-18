#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

#include "actors_base.h"
#include "def.h"

namespace Services
{
class Sprinkler_east : public ActorsBase {
public:
    Sprinkler_east() : ActorsBase(SPRINKLER_EAST, "sprinkler/east", 0) {}
};
} // end of namespace Services
