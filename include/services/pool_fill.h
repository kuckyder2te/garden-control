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
class Pool_fill : public ActorsBase {
public:
    Pool_fill() : ActorsBase(POOL_FILL, "pool/fill", 0) {}
};
} // end of namespace Services
