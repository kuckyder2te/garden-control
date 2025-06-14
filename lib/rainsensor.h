/*
    Filename: rainsensor.h
    Date: 2025.06.08
    Author: Wilhelm Kuckelsberg
    Description: Regen erkennen
*/

#pragma once
#ifndef RAINSENSOR_H
#define RAINSENSOR_H

#include <Arduino.h>
#include <TaskManager.h>

class rainSensor : public Task::Base
{

public:
    typedef struct
    {
        float temp;
        float humidity;
    } model_t;

private:
    bool b;
    model_t *_model;
    char result[50];

public:
    rainSensor(const String &name)
        : Task::Base(name), b(false)
    {
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, LOW);
    } /*--------------------------------------------------------------------------*/

    rainSensor *setModel(model_t *model)
    {
        _model = model;
        return this;
    }

    virtual void begin() override
    {

    } /*--------------------------------------------------------------------------*/

    virtual void update() override
    {
        uint16_t rainValue = analogRead(A0);

        Serial.print("Rain Value ");
        Serial.println(rainValue);

        // Serial.print("_model->humidity ");
        // Serial.println(_model->humidity);
        // Serial.print("_model->temp ");
        // Serial.println(_model->temp);
    } /*--------------------------------------------------------------------------*/
};

#endif // RAINSENSOR_H
