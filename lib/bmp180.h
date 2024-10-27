#pragma once
#ifndef BMP180_H // change depending on your class
#define BMP180_H // change depending on your class

#include <Arduino.h>
#include <TaskManager.h>
#include <SFE_BMP180.h>

SFE_BMP180 pressure;
// #define ALTITUDE 1655.0 // Altitude of SparkFun's HQ in Boulder, CO. in meters
#define ALTITUDE 60.0 // home altitude
// SDA GPIO04 D2
// SCL GPIO05 D1

class bmp180 : public Task::Base
{
public:
    typedef struct
    {
        double temp;
        double pressure;
        double pressureSealevel;
    } model_t;

private:
    bool b;
    model_t *_model;
    float temperature;
    double pressureAltitude;
    char result[50];

public:
    bmp180(const String &name)
        : Task::Base(name), b(false)
    {
    }
    bmp180 *setModel(model_t *model)
    {
        _model = model;
        return this;
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
        // Serial.print(ALTITUDE * 3.28084, 0);
        // Serial.println(" feet");

        status = pressure.startTemperature();
        if (status != 0)
        {
            // Wait for the measurement to complete:
            delay(status);

            // Retrieve the completed temperature measurement:
            // Note that the measurement is stored in the variable _model->temp.
            // Function returns 1 if successful, 0 if failure.

            status = pressure.getTemperature(bmp180::_model->temp);
            if (status != 0)
            {
                // Print out the measurement:
                Serial.print("temperature: ");
                Serial.print(_model->temp, 2);
                Serial.println(" deg C, ");
                // Serial.print((9.0 / 5.0) * _model->temp + 32.0, 2);
                // Serial.println(" deg F");

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
                    // Note that the measurement is stored in the variable _model->pressure.
                    // Note also that the function requires the previous temperature measurement (_model->temp).
                    // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
                    // Function returns 1 if successful, 0 if failure.

                    status = pressure.getPressure(_model->pressure, _model->temp);
                    if (status != 0)
                    {
                        // Print out the measurement:
                        Serial.print("absolute pressure: ");
                        Serial.print(_model->pressure, 2);
                        Serial.println(" mb, ");
                        // Serial.print(_model->pressure * 0.0295333727, 2);
                        // Serial.println(" inHg");

                        // The pressure sensor returns abolute pressure, which varies with altitude.
                        // To remove the effects of altitude, use the sealevel function and your current altitude.
                        // This number is commonly used in weather reports.
                        // Parameters: _model->pressure = absolute pressure in mb, ALTITUDE = current altitude in m.
                        // Result: p0 = sea-level compensated pressure in mb

                        _model->pressureSealevel = pressure.sealevel(_model->pressure, ALTITUDE); // we're at 1655 meters (Boulder, CO)
                        Serial.print("relative (sea-level) pressure: ");
                        Serial.print(_model->pressureSealevel, 2);
                        Serial.println(" mb, ");
                        // Serial.print(pressureSealevel * 0.0295333727, 2);
                        // Serial.println(" inHg");

                        // On the other hand, if you want to determine your altitude from the pressure reading,
                        // use the altitude function along with a baseline pressure (sea-level or other).
                        // Parameters: _model->pressure = absolute pressure in mb, p0 = baseline pressure in mb.
                        // Result: a = altitude in m.

                        pressureAltitude = pressure.altitude(_model->pressure, _model->pressureSealevel);
                        Serial.print("computed altitude: ");
                        Serial.print(pressureAltitude, 0);
                        Serial.println(" meters, ");
                        // Serial.print(pressureAltitude * 3.28084, 0);
                        // Serial.println(" feet");
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
};

#endif // BMP180_H
