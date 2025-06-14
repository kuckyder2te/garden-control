#pragma once
/*
    File name. interface.h
    Date: 2024.10.06
    Author: Stephan Scholz
    Description: Garden Control
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "..\lib\def.h"

namespace interface
{
    typedef struct
    {
        bool poolPump_state;          // Turns the pool pump on or off
        bool watering_valve_state;    // Opens the valve for garden irrigation
        bool poolwater_valve_state;   // Opens the valve for filling the pool
    } model_t;
}

extern char msg[50];
extern PubSubClient client;
extern interface::model_t *interface_model;

void pont_pump(bool option)
{
    interface_model->poolPump_state = option;
    if (option)
    {
        Serial.println("Pool Pump ON");
        digitalWrite(POOL_PUMP, LOW);
    }
    else
    {
        Serial.println("Pool Pump OFF");
        digitalWrite(POOL_PUMP, HIGH);
    }
    msg[0] = (option ? '1' : '0');
    msg[1] = 0;
    client.publish("outGarden/pool_pump/state", msg);
} /*--------------------------------------------------------------------------*/

void watering_valve(bool option)
{
    interface_model->watering_valve_state = option;
    if (option)
    {
        Serial.println("Watering Valve ON");
        digitalWrite(WATERING_VALVE, LOW);
    }
    else
    {
        Serial.println("Valve OFF");
        digitalWrite(WATERING_VALVE, HIGH);
    }
    msg[0] = (option ? '1' : '0');
    msg[1] = 0; // String end
    client.publish("outGarden/watering-valve/state", msg);
} /*--------------------------------------------------------------------------*/

void poolwater_valve(bool option)
{
    interface_model->poolwater_valve_state = option;
    if (option)
    {
        Serial.println("Pool Water Valve ON");
        digitalWrite(POOLWATER_VALVE, LOW);
    }
    else
    {
        Serial.println("Pool Water Valve OFF");
        digitalWrite(POOLWATER_VALVE, HIGH);
    }
    msg[0] = (option ? '1' : '0');
    msg[1] = 0; // String end
    client.publish("outGarden/poolwater-valve/state", msg);
} /*--------------------------------------------------------------------------*/
