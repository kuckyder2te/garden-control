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
            static float _garden_tmin = 1000;
            static float _garden_tmax = -1000;

            _sensor->requestTemperatures();
            float _current = _sensor->getTempCByIndex(0);

            if (_current == DEVICE_DISCONNECTED_C || _current == 85.0)
            {
                LOGGER_ERROR("Sensor error!");
                return;
            }

            LOGGER_NOTICE_FMT("Garden current: %.1f min = %.1f max = %.1f\n", _current, _garden_tmin, _garden_tmax);

            if (_current < _garden_tmin)
            {
                _garden_tmin = _current;
                sprintf(_msg, "{ \"value\":%.1f }", _garden_tmin);
                _network->pubMsg("outGarden/temp_min_garden", _msg);
                LOGGER_NOTICE_FMT("temp min: %.1f", _garden_tmin);
            }

            if (_current > _garden_tmax)
            {
                _garden_tmax = _current;
                sprintf(_msg, "{ \"value\":%.1f }", _garden_tmax);
                _network->pubMsg("outGarden/temp_max_garden", _msg);
                LOGGER_NOTICE_FMT("temp max: %.1f", _garden_tmax);
            }

            sprintf(_msg, "{ \"value\":%.1f }", _current);
            _network->pubMsg("outGarden/temp_current_garden", _msg);
        }
    };
} // end of namespace Services