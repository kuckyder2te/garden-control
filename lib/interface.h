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
        bool poolPump_state;
        bool watering_valve_state;
        bool poolwater_valve_state;
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
        digitalWrite(POOL_LED, HIGH);
    }
    else
    {
        Serial.println("Pool Pump OFF");
        digitalWrite(POOL_PUMP, HIGH);
        digitalWrite(POOL_LED, LOW);
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
        digitalWrite(WATERING_LED, HIGH);
    }
    else
    {
        Serial.println("Valve OFF");
        digitalWrite(WATERING_VALVE, HIGH);
        digitalWrite(WATERING_LED, LOW);
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
        Serial.println("Valve ON");
        digitalWrite(POOLWATER_VALVE, LOW);
        digitalWrite(POOLWATER_LED, HIGH);
    }
    else
    {
        Serial.println("Valve OFF");
        digitalWrite(POOLWATER_VALVE, HIGH);
        digitalWrite(POOLWATER_LED, LOW);
    }
    msg[0] = (option ? '1' : '0');
    msg[1] = 0; // String end
    client.publish("outGarden/poolwater-valve/state", msg);
} /*--------------------------------------------------------------------------*/
