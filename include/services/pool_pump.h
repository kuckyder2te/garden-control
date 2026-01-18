#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

#include "actors_base.h"
#include "def.h"

namespace Services {
class Pool_pump : public ActorsBase {
public:
    Pool_pump(const String taskName)
     : ActorsBase(taskName, POOL_PUMP, "pool/pump", TIMEOUT_HEATPUMP) {}
};
} // end of namespace Services
