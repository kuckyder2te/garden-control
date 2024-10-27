#pragma once
#ifndef DHT22_H // change depending on your class
#define DHT22_H // change depending on your class

#include <Arduino.h>
#include <TaskManager.h>
#include <DHTesp.h>

DHTesp dhtESP;
#define DHT_PIN 16 // D0  PullUp Resistor 4k7

class dht22 : public Task::Base
{
public:
    typedef struct{
        float temp;
        float humidity;
    }model_t;

private:
    bool b;
    model_t *_model;
    char result[50];

public:
    dht22(const String &name)
        : Task::Base(name), b(false)
    {
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, LOW);
    } /*--------------------------------------------------------------------------*/

    dht22* setModel(model_t *model){
        _model = model;
        return this;
    }

    virtual void begin() override
    {
        dhtESP.setup(DHT_PIN, DHTesp::DHT22);
        delay(dhtESP.getMinimumSamplingPeriod());

    } /*--------------------------------------------------------------------------*/

    virtual void update() override
    {
        _model->humidity = dhtESP.getHumidity();
        _model->temp = dhtESP.getTemperature();

        // Serial.print(dhtESP.getStatusString());
        // Serial.print("\t");
        // Serial.print(_model->humidity, 1);
        // Serial.print("\t\t");
        // Serial.print(_model->temp, 1);
        // Serial.print("\t\t");
        // Serial.print(dhtESP.toFahrenheit(_model->temp), 1);
        // Serial.print("\t\t");
        // Serial.print(dhtESP.computeHeatIndex(_model->temp, _model->humidity, false), 1);
        // Serial.print("\t\t");
        // Serial.println(dhtESP.computeHeatIndex(dhtESP.toFahrenheit(_model->temp), _model->humidity, true), 1);

        Serial.print("_model->humidity ");
        Serial.println(_model->humidity);
        Serial.print("_model->temp ");
        Serial.println(_model->temp);
    } /*--------------------------------------------------------------------------*/
};

#endif // DHT22_H
