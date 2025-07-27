/*
    Filename: output.h
    Date: 2025.07.25
    Author: Wilhelm Kuckelsberg
    Description: client.publish outputs
*/

#pragma once
#ifndef OUTPUT_H
#define OUTPUT_H

#include <Arduino.h>
#include <TaskManager.h>
#include <PubSubClient.h>

extern PubSubClient client;

class outPut : public Task::Base
{

public:
    typedef struct
    {
        int count;
    } model_t;

private:
    model_t *_model;

public:
    outPut(const String &name)
        : Task::Base(name)
    {

    } /*--------------------------------------------------------------------------*/

    outPut *setModel(model_t *model)
    {
        _model = model;
        return this;
    }

    virtual void begin() override
    {

    } /*--------------------------------------------------------------------------*/

    virtual void update() override
    {
        Serial.println("OutPut");
        client.publish("outGarden/pool-pump/state", String(model.interface.poolPump_state).c_str());
        client.publish("outGarden/watering-valve/state", String(model.interface.watering_valve_state).c_str());
        client.publish("outGarden/poolwater-valve/state", String(model.interface.poolwater_valve_state).c_str());

    } /*--------------------------------------------------------------------------*/
};

#endif // OUTPUT
