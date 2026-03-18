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
#include "../include/services/sprinkler_east.h"
#include "../include/services/sprinkler_west.h"
#include "../include/services/pool_fill.h"
#include "../include/services/pool_pump.h"

#include <ArduinoJson.h>
#include "secrets.h"
/// @endcond

Network *_network;
JsonDocument doc;

HardwareSerial *TestOutput = &Serial;
HardwareSerial *DebugOutput = &Serial;

MessageBroker msgBroker;

// test-----------------------------------------

void setup()
{
  delay(2000);
  Serial.begin(115200);
  DebugOutput->begin(DEBUG_SPEED);
  Logger::setOutputFunction(&MyLoggerOutput::localUdpLogger);
  //Logger::setOutputFunction(&MyLoggerOutput::willyUdpLogger);
  Logger::setLogLevel(Logger::DEBUG); // Muss immer einen Wert in platformio.ini haben (SILENT)
  delay(500);                         // For switching on Serial Monitor
  LOGGER_NOTICE_FMT("************************* Garden Service (%s) *************************", __TIMESTAMP__);
  LOGGER_NOTICE("Start building Garden Service");
  _network = new Network(SID, PW, HOSTNAME, MQTT, MessageBroker::callback);
  _network->begin(LOGGER, PORT_FOR_GARDENSERVICE);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  /* 12V Valves */
  Tasks.add<Services::Sprinkler_east>("sprinkler_east")
      ->startFps(10); // 10 Hz = alle 100ms;
  Tasks.add<Services::Sprinkler_west>("sprinkler_west")
      ->startFps(10); // 10 Hz = alle 100ms;
  Tasks.add<Services::Pool_fill>("pool_fill")
      ->startFps(10); // 10 Hz = alle 100ms;

  /* 220V Pump */
  Tasks.add<Services::Pool_pump>("pool_pump")
      ->startFps(10); // 10 Hz = alle 100ms;
      
  Tasks.add<Services::Temperature>("temperature")
      ->init(DALLAS)
      ->startFps(0.017); // 0,017 ~ 1 minute

    delay(2000);
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
    digitalWrite(LED_BUILTIN, lastState);
    lastState = !lastState;
    lastMillis = millis();
  }
}
/*--------------------------------------------------------------------------*/
