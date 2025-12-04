#pragma once
/// @cond
#include <Arduino.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

#include <TaskManager.h>
#include "../network.h"
#include <Wire.h>
#include <DallasTemperature.h>

extern Network *_network;

namespace Services
{
    class Temperature : public Task::Base
    {
        OneWire *_interface;
        DallasTemperature *_sensor;
        char _msg[30];
        float _temp_current;

    public:
        Temperature(const String &name) : Task::Base(name)
        {
            LOGGER_NOTICE("Create temperature task");
        }

        virtual void begin() override
        {
            LOGGER_NOTICE("Dallas Temperature IC Control Library Demo");
        }

        Temperature *init(const uint8_t DallasPin)
        {
            LOGGER_VERBOSE("enter ...");
            _interface = new OneWire(DallasPin);
            _sensor = new DallasTemperature(_interface);
            _sensor->begin();
            LOGGER_VERBOSE("leave ...");
            return this;
        }

        virtual void update() override
        {
            static float tmin = 1000;
            static float tmax = -1000;

            _sensor->requestTemperatures();
            float t = _sensor->getTempCByIndex(0);

            if (t == DEVICE_DISCONNECTED_C || t == 85.0)
            {
                LOGGER_ERROR("Sensor error!");
                return;
            }

            _temp_current = t;

            Serial.printf("TEST: curr=%.1f  min=%.1f  max=%.1f\n", t, tmin, tmax);

            // MIN
            if (t < tmin)
            {
                tmin = t;
                sprintf(_msg, "{ \"value\":%.1f }", tmin);
                _network->pubMsg("outGarden/temp_min", _msg);
                LOGGER_NOTICE_FMT("temp min: %.1f", tmin);
            }

            // MAX
            if (t > tmax)
            {
                tmax = t;
                sprintf(_msg, "{ \"value\":%.1f }", tmax);
                _network->pubMsg("outGarden/temp_max", _msg);
                LOGGER_NOTICE_FMT("temp max: %.1f", tmax);
            }

            // Always publish current
            sprintf(_msg, "{ \"value\":%.1f }", t);
            _network->pubMsg("outGarden/temp_current", _msg);
        }
    };
} // end of namespace Services