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
    bool b;
    float humidity;
    float temperature;
    char result[50];

public:
    dht22(const String &name)
        : Task::Base(name), b(false)
    {
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, LOW);
    } /*--------------------------------------------------------------------------*/

    virtual void begin() override
    {
        dhtESP.setup(DHT_PIN, DHTesp::DHT22);
        delay(dhtESP.getMinimumSamplingPeriod());

    } /*--------------------------------------------------------------------------*/

    virtual void update() override
    {
        humidity = dhtESP.getHumidity();
        temperature = dhtESP.getTemperature();

        // Serial.print(dhtESP.getStatusString());
        // Serial.print("\t");
        // Serial.print(humidity, 1);
        // Serial.print("\t\t");
        // Serial.print(temperature, 1);
        // Serial.print("\t\t");
        // Serial.print(dhtESP.toFahrenheit(temperature), 1);
        // Serial.print("\t\t");
        // Serial.print(dhtESP.computeHeatIndex(temperature, humidity, false), 1);
        // Serial.print("\t\t");
        // Serial.println(dhtESP.computeHeatIndex(dhtESP.toFahrenheit(temperature), humidity, true), 1);

        Serial.print("Humidity ");
        Serial.println(humidity);
        Serial.print("Temperature ");
        Serial.println(temperature);
    } /*--------------------------------------------------------------------------*/

    char *getTemperature()
    {
        dtostrf(temperature, 10, 1, result);
        return result;
    } /*--------------------------------------------------------------------------*/

    char *getHumidity()
    {
        dtostrf(humidity, 10, 1, result);
        return result;
    } /*--------------------------------------------------------------------------*/
};

#endif // DHT22_H
