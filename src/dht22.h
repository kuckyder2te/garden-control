#pragma once
#ifndef DHT22_H // change depending on your class
#define DHT22_H // change depending on your class

#include <Arduino.h>
#include <TaskManager.h>
#include <DHTesp.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // <- change to your own led pin
#endif

DHTesp dht;
#define DHT_PIN 16 // D0  PullUp Resistor 4k7

class dht22 : public Task::Base
{
    bool b;
    boolean dhtInit = false;
    float humidity;
    float temperature;
    char result[50];

public:
    dht22(const String &name)
        : Task::Base(name), b(false)
    {
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, LOW);
    }

    virtual void begin() override
    {
        dht.setup(DHT_PIN, DHTesp::DHT22);
        delay(dht.getMinimumSamplingPeriod());

        dht.setup(DHT_PIN, DHTesp::DHT22);
        dhtInit = true;

        humidity = dht.getHumidity();
        temperature = dht.getTemperature();

        // Serial.print(dht.getStatusString());
        // Serial.print("\t");
        // Serial.print(humidity, 1);
        // Serial.print("\t\t");
        // Serial.print(temperature, 1);
        // Serial.print("\t\t");
        // Serial.print(dht.toFahrenheit(temperature), 1);
        // Serial.print("\t\t");
        // Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
        // Serial.print("\t\t");
        // Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);

        dtostrf(humidity, 10, 1, result);
        Serial.print("Humidity ");
        Serial.println(humidity);
        dtostrf(temperature, 10, 1, result);
        Serial.print("Temperature ");
        Serial.println(temperature);
    } /*--------------------------------------------------------------------------*/

    virtual void update() override
    {
        dht.setup(DHT_PIN, DHTesp::DHT22);
        delay(dht.getMinimumSamplingPeriod());

        humidity = dht.getHumidity();
        temperature = dht.getTemperature();

        // Serial.print(dht.getStatusString());
        // Serial.print("\t");
        // Serial.print(humidity, 1);
        // Serial.print("\t\t");
        // Serial.print(temperature, 1);
        // Serial.print("\t\t");
        // Serial.print(dht.toFahrenheit(temperature), 1);
        // Serial.print("\t\t");
        // Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
        // Serial.print("\t\t");
        // Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);

        dtostrf(humidity, 10, 1, result);
        Serial.print("Humidity ");
        Serial.println(humidity);
        dtostrf(temperature, 10, 1, result);
        Serial.print("Temperature ");
        Serial.println(temperature);
    } /*--------------------------------------------------------------------------*/
    
    char *getTemperature()
    {
        return dtostrf(temperature, 10, 1, result);
    } /*--------------------------------------------------------------------------*/

    float getHumidity()
    {
        return humidity;
    } /*--------------------------------------------------------------------------*/
};

#endif // DHT22_H
