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
            _sensor->requestTemperatures();
            float _temperature = _sensor->getTempCByIndex(0);

            if (_temperature == DEVICE_DISCONNECTED_C || _temperature == 85.0)
            {
                LOGGER_ERROR("Sensor error!");
                return;
            }

            LOGGER_NOTICE_FMT("Garden current: %.1f \n", _temperature);

            sprintf(_msg, "{ \"value\":%.1f }", _temperature);
            _network->pubMsg("outGarden/garden/temperature", _msg);
        }
    };
} // end of namespace Services