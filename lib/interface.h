#pragma once
/*
    File name. interface.h
    Date: 2024.10.06
    Author: Stephan Scholz / Wilhelm Kuckelsberg
    Description: Garden Control

    Controls the pool pump on a time-controlled basis.
    Turns on the rinse water and manual filling of the pool.
    Controls the garden irrigation on a time-controlled basis.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "..\lib\def.h"

bool poolPump_state;          // Turns the pool pump on or off
bool watering_terrace_state;  // Opens the valve for garden irrigation
bool watering_garden_state;   // Opens the valve for garden irrigation
bool poolwater_valve_state;   // Opens the valve for filling the pool

extern char msg[50];
extern PubSubClient client;

void pool_pump(bool option)
{
    poolPump_state = option;
    if (option)
    {
        Serial.println("Pool Pump ON");
        digitalWrite(POOL_PUMP, HIGH);
    }
    else
    {
        Serial.println("Pool Pump OFF");
        digitalWrite(POOL_PUMP, LOW);
    }
    msg[0] = (option ? '1' : '0');
    msg[1] = 0;
    client.publish("outGarden/pool_pump/state", msg);
} /*--------------------------------------------------------------------------*/

void watering_terrace(bool option)
{
    watering_terrace_state = option;
    if (option)
    {
        Serial.println("Watering terrace ON");
        digitalWrite(WATERING_TERRACE, HIGH);
    }
    else
    {
        Serial.println("Watering terrace OFF");
        digitalWrite(WATERING_TERRACE, LOW);
    }
    msg[0] = (option ? '1' : '0');
    msg[1] = 0; // String end
    client.publish("outGarden/watering_terrace/state", msg);
} /*--------------------------------------------------------------------------*/

void watering_garden(bool option)
{
    watering_garden_state = option;
    if (option)
    {
        Serial.println("Watering garden ON");
        digitalWrite(WATERING_GARDEN, HIGH);
    }
    else
    {
        Serial.println("Watering garden OFF");
        digitalWrite(WATERING_GARDEN, LOW);
    }
    msg[0] = (option ? '1' : '0');
    msg[1] = 0; // String end
    client.publish("outGarden/watering_garden/state", msg);
} /*--------------------------------------------------------------------------*/

void poolwater_valve(bool option)
{
    poolwater_valve_state = option;
    if (option)
    {
        Serial.println("Pool Water Valve ON");
        digitalWrite(POOLWATER_VALVE, HIGH);
    }
    else
    {
        Serial.println("Pool Water Valve OFF");
        digitalWrite(POOLWATER_VALVE, LOW);
    }
    msg[0] = (option ? '1' : '0');
    msg[1] = 0; // String end
    client.publish("outGarden/poolwater_valve/state", msg);
} /*--------------------------------------------------------------------------*/
