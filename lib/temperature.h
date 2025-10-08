/*
    File name: temperature.h
    Date: 2025.01.07
    Author: Wilhelm Kuckelsberg
    Description: Temperature measurement with Dallas DS18B20
*/

#pragma once
#ifndef TEMPERATURE_TASK_H
#define TEMPERATURE_TASK_H

#include <Arduino.h>
#include <TaskManager.h>
#include <PubSubClient.h>
#include "..\lib\def.h"

OneWire oneWire(DALLAS);
DallasTemperature sensors(&oneWire);

class temperature : public Task::Base
{
    PubSubClient *_client;
    char msg[30];
    float temperatureGround;

public:
    temperature(const String &name)
        : Task::Base(name)
    {
    }

    temperature *setClient(PubSubClient *client)
    {
        _client = client;
        return this;
    }

    virtual void begin() override
    {
        Serial.println("Dallas Temperature IC Control Library Demo");
        sensors.begin();
    }

    virtual void update() override
    {
        sensors.requestTemperatures(); // Send the command to get temperatures
        temperatureGround = sensors.getTempCByIndex(0);

        if (temperatureGround != DEVICE_DISCONNECTED_C)
        {
            sprintf(msg, "{ \"value\":%.1f }", temperatureGround);
            _client->publish("outGarden/temperaturePool", msg);

            sprintf(msg,"Temperature in ground %0.1f", temperatureGround);
            Serial.println(msg);
        }
        else
        {
            Serial.println("Error: Could not read temperature data");
        }
    }
};
#endif // TEMPERATURE_TASK_H