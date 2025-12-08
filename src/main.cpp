/*
File name: main.cpp
Author:    Stefan Scholz / Wilhelm Kuckelsberg
Date:      2024.10.10
Project:   Garden Control
*/
///@cond
#include <Arduino.h>
#include <TaskManager.h>
#include "def.h"

#define LOCAL_DEBUG
char logBuf[DEBUG_MESSAGE_BUFFER_SIZE];

#include "../include/myLogger.h"

#include "../include/network.h"
#include "../include/messageBroker.h"

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <Wire.h>

#include "../include/services/temperature.h"
#include "../include/services/rainfall.h"
#include "../include/services/valve_garden.h"
#include "../include/services/valve_terrace.h"
#include "../include/services/valve_rinse.h"
#include "../include/services/pool_pump.h"

#include <ArduinoJson.h>
#include "secrets.h"
/// @endcond

Network *_network;
JsonDocument doc;

HardwareSerial *TestOutput = &Serial;
HardwareSerial *DebugOutput = &Serial;

MessageBroker msgBroker;

Services::Valve_garden *ValveGarden;
Services::Valve_terrace *ValveTerrace;
Services::Valve_rinse *ValveRinse;

Services::Pool_pump *PoolPump;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

void setup()
{
  delay(2000);
  Serial.begin(115200);
  DebugOutput->begin(DEBUG_SPEED);
  Logger::setOutputFunction(&MyLoggerOutput::localLogger);
  Logger::setLogLevel(Logger::DEBUG); // Muss immer einen Wert in platformio.ini haben (SILENT)
  delay(500);                         // For switching on Serial Monitor
  LOGGER_NOTICE_FMT("************************* Garden Service (%s) *************************", __TIMESTAMP__);
  LOGGER_NOTICE("Start building Garden Service");
  _network = new Network(SID, PW, HOSTNAME, MQTT, MessageBroker::callback);
  _network->begin(MQTT, PORT_FOR_GARDENSERVICE);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  /* 12V Valves */
  ValveGarden = new Services::Valve_garden(VALVE_GARDEN, 200, 10000);
  ValveTerrace = new Services::Valve_terrace(VALVE_TERRACE, 200, 10000);
  ValveRinse = new Services::Valve_rinse(VALVE_RINSE, 200, 10000);

  /* 220V Pump */
  PoolPump = new Services::Pool_pump(POOL_PUMP, 200, 5000); // 5 s timeout

  Tasks.add<Services::Temperature>("temperature")
      ->init(DALLAS)
      ->startFps(0.017); // 0,017 ~ 1 minute

  msgBroker.printTopics();
  LOGGER_NOTICE("Finished building Garden Service. Will enter infinite loop");

} /*--------------------------------------------------------------------------*/

void loop()
{
  static unsigned long lastMillis;
  static bool lastState = LOW;
  _network->update();

  Tasks.update();

  if (millis() - lastMillis >= 1000) // This can also be used to test the main loop.
  {

    ValveGarden->update();
    ValveTerrace->update();
    ValveRinse->update();

    PoolPump->update();

    digitalWrite(LED_BUILTIN, lastState);
    lastState = !lastState;
    lastMillis = millis();
  }
}
/*--------------------------------------------------------------------------*/
