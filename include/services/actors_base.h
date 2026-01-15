#pragma once
/// @cond
#include <Arduino.h>
#include <ArduinoJson.h>
#define LOCAL_DEBUG
#include "myLogger.h"
/// @endcond

#include "../message.h"
#include "../messageBroker.h"
#include "def.h"

namespace Services
{
    class ActorsBase
    {
    protected:
        uint8_t _pump_pin;
        String _rootTopic;
        bool _state = false;
        unsigned long _timeoutMs = 0;    // 0 = disable
        unsigned long _onSince = 0;      // Time from which the pump is ON

        String _topic_prefix; // z.B. "hcl_pump"

    private:
        class StateMsg : public Message
        {
            ActorsBase &_parent;

        public:
            StateMsg(ActorsBase &parent, const String &topic)
                : Message(topic), _parent(parent) {}

            bool call(JsonDocument payload) override
            {
                return _parent.onMessage(payload);
            }
        };

    public:
        ActorsBase(uint8_t pumpPin,
                    const String &rootTopic,   
                    
                    unsigned long timeoutMs = 0)
            : _pump_pin(pumpPin),
              _rootTopic(rootTopic),
              
              _timeoutMs(timeoutMs)
        {
            LOGGER_NOTICE_FMT("Create vales '%s' on pin %d", _rootTopic.c_str(), _pump_pin);

            pinMode(_pump_pin, OUTPUT);
            digitalWrite(_pump_pin, LOW);

            msgBroker.registerMessage(new StateMsg(*this, _rootTopic + "/state"));
        }

        virtual ~ActorsBase() = default;

        // ----------------------------------------------------------
        // Processing MQTT messages
        // ----------------------------------------------------------
        bool onMessage(JsonDocument payload)
        {
            // Accept only boolean
            if (payload.is<bool>())
            {
                setState(payload.as<bool>());
                publishState();
                return true;
            }

            LOGGER_WARNING_FMT("%s: Payload not bool", _rootTopic.c_str());
            return false;
        }

        // Must be called periodically (e.g., in loop()).
        virtual void update(unsigned long now = millis())
        {
            if (_state && _timeoutMs > 0 && _onSince > 0)
            {
                if (now - _onSince >= _timeoutMs)
                {
                    LOGGER_NOTICE_FMT("%s: timeout reached (%lums)", _rootTopic.c_str(), _timeoutMs);

                    setState(false);
                    publishState();
                }
            }
        }

        // ----------------------------------------------------------
        // Switch ON/OFF
        // ----------------------------------------------------------
        virtual void setState(bool on)
        {
            _state = on;
            digitalWrite(_pump_pin, on ? HIGH : LOW);

            LOGGER_NOTICE_FMT("%s %s (pin %d)", _rootTopic.c_str(), on ? "ON" : "OFF", _pump_pin);
        }

    protected:
        // ----------------------------------------------------------
        // Puplish state wia MQTT
        // ----------------------------------------------------------
        void publishState()
        {
            JsonDocument doc;
            doc.set(_state);
            _network->pubMsg((_rootTopic + "/state").c_str(), doc);
        }
    };

} // end of namespace Services
