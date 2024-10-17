#pragma once
#ifndef BMP180_H // change depending on your class
#define BMP180_H // change depending on your class

#include <Arduino.h>
#include <TaskManager.h>

#include <SFE_BMP180.h>
#include <Wire.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // <- change to your own led pin
#endif

SFE_BMP180 pressure;
// #define ALTITUDE 1655.0 // Altitude of SparkFun's HQ in Boulder, CO. in meters
#define ALTITUDE 60.0 // home altitude
// SDA GPIO04 D2
// SCL GPIO05 D1

class bmp180 : public Task::Base
{
    bool b;

    //    float humidity;
    float temperature;
    // double pressure;
    float luftdruck = 995.0;       // in hPa    only test values
    float temperatur = 18.5;       // in °C
    float luftfeuchtigkeit = 85.0; // in %

    //   double p0;
    double pressureSealevel;
    double pressureAltitude;
    double T;
    double P;

    char result[50];

public:
    bmp180(const String &name)
        : Task::Base(name), b(false)
    {
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, LOW);
    }

    virtual void begin() override
    {

        if (pressure.begin())
            Serial.println("BMP180 init success");
        else
        {
            Serial.println("BMP180 init fail\n\n");
            while (1)
                ;
        }
    } /*--------------------------------------------------------------------------*/

    virtual void update() override
    {
        char status;

        Serial.println();
        Serial.print("provided altitude: ");
        Serial.print(ALTITUDE, 0);
        Serial.print(" meters, ");
        Serial.print(ALTITUDE * 3.28084, 0);
        Serial.println(" feet");

        status = pressure.startTemperature();
        if (status != 0)
        {
            // Wait for the measurement to complete:
            delay(status);

            // Retrieve the completed temperature measurement:
            // Note that the measurement is stored in the variable T.
            // Function returns 1 if successful, 0 if failure.

            status = pressure.getTemperature(T);
            if (status != 0)
            {
                // Print out the measurement:
                Serial.print("temperature: ");
                Serial.print(T, 2);
                Serial.print(" deg C, ");
                Serial.print((9.0 / 5.0) * T + 32.0, 2);
                Serial.println(" deg F");

                // Start a pressure measurement:
                // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
                // If request is successful, the number of ms to wait is returned.
                // If request is unsuccessful, 0 is returned.

                status = pressure.startPressure(3);
                if (status != 0)
                {
                    // Wait for the measurement to complete:
                    delay(status);

                    // Retrieve the completed pressure measurement:
                    // Note that the measurement is stored in the variable P.
                    // Note also that the function requires the previous temperature measurement (T).
                    // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
                    // Function returns 1 if successful, 0 if failure.

                    status = pressure.getPressure(P, T);
                    if (status != 0)
                    {
                        // Print out the measurement:
                        Serial.print("absolute pressure: ");
                        Serial.print(P, 2);
                        Serial.print(" mb, ");
                        Serial.print(P * 0.0295333727, 2);
                        Serial.println(" inHg");

                        // The pressure sensor returns abolute pressure, which varies with altitude.
                        // To remove the effects of altitude, use the sealevel function and your current altitude.
                        // This number is commonly used in weather reports.
                        // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
                        // Result: p0 = sea-level compensated pressure in mb

                        pressureSealevel = pressure.sealevel(P, ALTITUDE); // we're at 1655 meters (Boulder, CO)
                        Serial.print("relative (sea-level) pressure: ");
                        Serial.print(pressureSealevel, 2);
                        Serial.print(" mb, ");
                        Serial.print(pressureSealevel * 0.0295333727, 2);
                        Serial.println(" inHg");

                        // On the other hand, if you want to determine your altitude from the pressure reading,
                        // use the altitude function along with a baseline pressure (sea-level or other).
                        // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
                        // Result: a = altitude in m.

                        pressureAltitude = pressure.altitude(P, pressureSealevel);
                        Serial.print("computed altitude: ");
                        Serial.print(pressureAltitude, 0);
                        Serial.print(" meters, ");
                        Serial.print(pressureAltitude * 3.28084, 0);
                        Serial.println(" feet");
                    }
                    else
                        Serial.println("error retrieving pressure measurement\n");
                }
                else
                    Serial.println("error starting pressure measurement\n");
            }
            else
                Serial.println("error retrieving temperature measurement\n");
        }
        else
            Serial.println("error starting temperature measurement\n");
    } /*--------------------------------------------------------------------------*/

    float getTemperature()
    {
        return temperature;

    } /*--------------------------------------------------------------------------*/

    float getPressureSealevel()
    {
        return pressureSealevel;

    } /*--------------------------------------------------------------------------*/
};

#endif // BMP180_H
