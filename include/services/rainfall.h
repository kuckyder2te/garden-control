#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

class Rainfall
{
private:
  
public:
  Rainfall() {}
  /*--------------------------------------------------------------------------*/
  bool update()
  {
    static bool lastState = false;
    static int debugCount = 0;
    bool impulse = false;
    static unsigned long lastEvent = millis();
    if (!digitalRead(RAINDROP_PIN) && !lastState)
    {
      lastState = true;
      impulse = true;
      lastEvent = millis();
      LOGGER_NOTICE_FMT("Rainfall event No: %d", debugCount);
      // Serial.print("Rainfall event No ");Serial.println(debugCount);
      debugCount++;
    };
    if (millis() - 500 >= lastEvent)
    {
      if (digitalRead(RAINDROP_PIN))
      {
        lastState = false;
      }
    }
    return impulse;
  }
}; // RAINFALL_H