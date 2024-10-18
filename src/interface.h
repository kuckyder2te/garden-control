/*
    File name. interface.h
    Date: 2024.10.06
    Author: Stephan Scholz
    Description: Pool Control
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// #define POOL_LED_GREEN 1 // D1
// #define POOL_LED_RED 3  // D0
#define POOL_SWT 12      // D5

#define WATERING_LED_GREEN 13 // D2
#define WATERING_LED_RED 15   // D3
#define WATERING_SWT 14      // D6

// #define TEMPERATURE A0
extern char msg[50];
extern PubSubClient client;

void pool_pump(bool option)
{
    if (option)
    {
        Serial.println("Pump ON");
        digitalWrite(POOL_SWT, HIGH);
        digitalWrite(13, LOW);
        // digitalWrite(POOL_LED_RED, HIGH);
    }
    else
    {
        Serial.println("Pump OFF");
        digitalWrite(POOL_SWT, LOW);
        digitalWrite(13, HIGH);
        // digitalWrite(POOL_LED_RED, LOW);  
    }
    msg[0] = (option ? '1' : '0');
    msg[1] = 0;
    client.publish("outGarden/pool/pump", msg);
}/*--------------------------------------------------------------------------*/

void watering(bool option)
{
    if (option)
    {
        Serial.println("Watering ON");
        digitalWrite(WATERING_SWT, HIGH);
        digitalWrite(15, HIGH);
        // digitalWrite(WATERING_LED_RED, LOW);
    }
    else
    {
        Serial.println("Watering OFF");
        digitalWrite(WATERING_SWT, LOW);
        digitalWrite(15, LOW);
        // digitalWrite(WATERING_LED_GREEN, LOW);
    }
    msg[0] = (option ? '1' : '0');
    msg[1] = 0; // String end
    client.publish("outGarden/watering/state", msg);
}/*--------------------------------------------------------------------------*/
