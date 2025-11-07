#pragma once
/*
    File name. rainfall.h
    Date: 2025.08.22
    Author: Wilhelm Kuckelsberg
    Description: Garden Control

    Measuring the amount of rain per unit of time.
*/

#ifndef RAINFALL_H
#define RAINFALL_H

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
      Serial.print("Rainfall event No ");Serial.println(debugCount);
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
};

#endif // RAINFALL_H