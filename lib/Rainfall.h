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
  float wert;

  //  int triggerPin = 14; // The stick reading about it tipped.
  int totalCount = 0; // a simple calculator that adds how many tipping it has done since its start.

  const char *mmPerSquareMeter = "0.094175"; /* Original Text
                                                1.25 ml per dipper change, 94.175 ml per m^2 (=1.25*75.34), = 0.094175mm/m^2
                                                diameter of raingauge is about 130mm, r = 65mm, surface = pi*r^2 = 132.73 cm^2
                                                therefore per m^2 (=10000cm^2) we have a factor of about 75.34
                                             */
                                             /*
                                             Realer Test 20 Dipper change ~96g
                                             1 Dipper change = L/R ~ 4,8g
                                             Umschwingverhältnis Links zu Rechts 1,32-1,38 sec
                                             */

  bool lastState = false;        // Specifies which mode was last time.
  bool currentState = false;     // Specifies the state of the loop now
  unsigned long lastChanged = 0; // when did we change the status the last time

public:
  Rainfall() {}
  /*--------------------------------------------------------------------------*/
  bool update()
  {
    static bool lastState = false;
    static int debugCount = 0;
    bool impulse = false;
    static unsigned long lastEvent = millis();
    if (!digitalRead(TRIGGER_PIN) && !lastState)
    {
      lastState = true;
      impulse = true;
      lastEvent = millis();
      Serial.print("Rainfall event No ");Serial.println(debugCount);
      debugCount++;
    };
    if (millis() - 500 >= lastEvent)
    {
      if (digitalRead(TRIGGER_PIN))
      {
        lastState = false;
      }
    }
    return impulse;
  }
};

#endif // RAINFALL_H