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
class Sprinkler_west : public ActorsBase {
public:
    Sprinkler_west() : ActorsBase(SPRINKLER_WEST, "sprinkler/west", 0) {}
};
} // end of namespace Services
