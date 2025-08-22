#pragma once
/*
    File name. Rainfall.h
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
  bool firstLoop = true; // Indicates that this is the first loop

//  int triggerPin = 14; // The stick reading about it tipped.
  int totalCount = 0;  // a simple calculator that adds how many tipping it has done since its start.

  const char *mmPerSquareMeter = "0.094175"; // 1.25 ml per dipper change, 94.175 ml per m^2 (=1.25*75.34), = 0.094175mm/m^2
                                             // diameter of raingauge is about 130mm, r = 65mm, surface = pi*r^2 = 132.73 cm^2
                                             // therefore per m^2 (=10000cm^2) we have a factor of about 75.34

  bool lastState = false;        // Specifies which mode was last time.
  bool currentState = false;     // Specifies the state of the loop now
  unsigned long lastChanged = 0; // when did we change the status the last time

public:
  Rainfall() {}

  float getWert() /*const*/
  {
    int currentState = digitalRead(TRIGGER_PIN);
    if (firstLoop)
    {
      lastState = currentState;
      firstLoop = false; // Sets that it has now run the startup.
    }; //  If it is the first startup then it should not send a dump

    // Checks the value of the stick, translates it to a bool value.
    if (currentState == 1)
    {
      currentState = true;
    }
    else
    {
      currentState = false;
    }

    // Checks whether the previous loop has the same loop as the current loop. Will not go into this first loop.
    if (lastState != currentState)
    {
      lastState = currentState;

      // between two changes, there should be 1 sec difference
      if ((millis() - lastChanged) > 1000)
      {
        totalCount++;
        lastChanged = millis();
      //  client.publish(MQTT_CHANNEL, mmPerSquareMeter);
        Serial.print("Number of times:");
        Serial.println(totalCount);
      }
    }

    delay(150); // small delay to avoid false readings. ?????

    return totalCount;
  }

  void setWert(float neuerWert)
  {
    wert = neuerWert;
  }
};

#endif  // RAINFALL_H