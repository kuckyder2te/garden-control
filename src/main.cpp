/*
File name: main.cpp
Author:    Stefan Scholz / Wilhelm Kuckelsberg
Date:      2024.10.10
Project:   Garden Control
*/

#include <Arduino.h>
#include <TaskManager.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "..\lib\model.h"
#include "..\lib\interface.h"
#include "..\lib\secrets.h"
#include "..\lib\def.h"
#include "..\lib\rainfall.h"
#include "..\lib\output.h"

const char *ssid = SID;
const char *password = PW;
const char *mqtt_server = MQTT;

WiFiClient espClient;
PubSubClient client(espClient);
JsonDocument doc;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

bool MAIN_LED_STATE = false;

// ----- OTA begin --------
#include <ElegantOTA.h>

AsyncWebServer server(80);

unsigned long ota_progress_millis = 0;

void onOTAStart()
{
  Serial.println("OTA update started!");
}

void onOTAProgress(size_t current, size_t final)
{
  if (millis() - ota_progress_millis > 1000)
  {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success)
{
  if (success)
  {
    Serial.println("OTA update finished successfully!");
  }
  else
  {
    Serial.println("There was an error during OTA update!");
  }
}
// ----- OTA end --------

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
} /*--------------------------------------------------------------------------*/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String topicStr(topic); // macht aus dem Topic ein String -> topicStr
  if (topicStr.indexOf('/') >= 0)
  /*prüft ob die Nachricht ein / enthält was ja den Pfad des Topics aufteilt
  und mindestens eins sollte bei inPump/Egon ja drin sein
  */
  {
    // Serial.print("topic = ");Serial.println(topic);
    //  The topic includes a '/', we'll try to read the number of bottles from just after that
    topicStr.remove(0, topicStr.indexOf('/') + 1);
    /*
      löscht inPump/ so dass in topicStr nur noch Egon übrig bleibt
    */
    if (topicStr.indexOf('/') >= 0)
    {
      String rootStr = topicStr.substring(0, topicStr.indexOf('/'));
      Serial.println(rootStr);
      if (rootStr == "pool_pump")
      {
        switch (payload[0])
        {
        case '0': // false
          // Pump off
          pont_pump(false);
          break;
        case '1':
          // Pump on
          pont_pump(true);
          break;
        default:
          // Warning !! Undefined payload or not 1/0
          break;
        }
      }
      else if (rootStr == "watering_valve")
      {
        switch ((char)payload[0])
        {
        case '0':
          watering_valve(false);
          break;
        case '1':
          watering_valve(true);
          break;
        default:
          // Warning !! Undefined payload or not 1/0
          break;
        }
      }
      else if (rootStr == "poolwater_valve")
      {
        switch ((char)payload[0])
        {
        case '0':
          poolwater_valve(false);
          break;
        case '1':
          poolwater_valve(true);
          break;
        default:
          // Warning !! Undefined payload or not 1/0
          break;
        }
      }
      else
      {
        Serial.println("Unknown topic");
      }
    }
  }
} /*--------------------------------------------------------------------------*/

// Checks if motion was detected, sets LED HIGH and starts a timer
// IRAM_ATTR void detectsMovement()
// {
//   Serial.println("MOTION DETECTED!!!");
//   digitalWrite(LED_BUILTIN, HIGH);
//   model.count++;
//   digitalWrite(LED_BUILTIN, LOW);

// } /*--------------------------------------------------------------------------*/

void setup()
{
  delay(2000);
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(POOL_PUMP, OUTPUT_OPEN_DRAIN);
  digitalWrite(POOL_PUMP, HIGH);

  pinMode(WATERING_VALVE, OUTPUT_OPEN_DRAIN);
  digitalWrite(WATERING_VALVE, HIGH);

  pinMode(POOLWATER_VALVE, OUTPUT_OPEN_DRAIN);
  digitalWrite(POOLWATER_VALVE, HIGH);

  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  // digitalWrite(TRIGGER_PIN, LOW);

  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  // attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), detectsMovement, RISING);

  Serial.println();
  Serial.println("Garden control is started");
  String thisBoard = ARDUINO_BOARD;
  Serial.println(thisBoard);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Tasks.add<rainFall>("rainFall")
      ->setClient(&client)
      ->startFps(0.1);

  Tasks.add<outPut>("output")
      ->startFps(0.1);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "Garden-Service"); });

  ElegantOTA.begin(&server); // Start ElegantOTA
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);
  server.begin();
  Serial.println("HTTP server started");

} /*--------------------------------------------------------------------------*/

bool reconnect()
{
  Serial.print("Attempting MQTT connection...");
  String clientId = "PumpValveNode-";
  clientId += String(random(0xffff), HEX);

  if (client.connect(clientId.c_str()))
  {
    Serial.println("connected");
    client.publish("outGarden", "{\"msg\":\"Reconnect: Pool Pump and Valve\"}");
    client.subscribe("inGarden/#");
    return true;
  }
  else
  {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    return false;
  }
} /*--------------------------------------------------------------------------*/

void loop()
{
  static unsigned long lastMillis = millis();
  // uint16_t elapsed_time = 10000; // 10 sec
  static bool TriggerState = digitalRead(TRIGGER_PIN);

  Tasks.update();

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  if (digitalRead(TRIGGER_PIN) != TriggerState)
  {
    TriggerState = digitalRead(TRIGGER_PIN);
    model.count++;
    client.publish("outGarden/rainSensor/trip", "1");
  }

  if (millis() - lastMillis >= 1000)
  {
    digitalWrite(LED_BUILTIN, MAIN_LED_STATE);
    MAIN_LED_STATE =! MAIN_LED_STATE;
    lastMillis = millis();
  }
} /*--------------------------------------------------------------------------*/
