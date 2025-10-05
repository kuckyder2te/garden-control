/*
File name: main.cpp
Author:    Stefan Scholz / Wilhelm Kuckelsberg
Date:      2024.10.10
Project:   Garden Control
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include "..\lib\interface.h"
#include "..\lib\secrets.h"
#include "..\lib\def.h"
#include "..\lib\rainfall.h"
#include "..\lib\temperature.h"

const char *ssid = SID;
const char *password = PW;
const char *mqtt_server = MQTT;

//const char *mmPerSquareMeter = "0.094175";

WiFiClient espClient;
PubSubClient client(espClient);
JsonDocument doc;
Rainfall rainfall;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

bool MAIN_LED_STATE = false;

uint16 rain_counter = 0;

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
          pool_pump(false);
          break;
        case '1':
          // Pump on
          pool_pump(true);
          break;
        default:
          // Warning !! Undefined payload or not 1/0
          break;
        }
      }
      else if (rootStr == "watering_terrace")
      {
        switch ((char)payload[0])
        {
        case '0':
          watering_terrace(false);
          break;
        case '1':
          watering_terrace(true);
          break;
        default:
          // Warning !! Undefined payload or not 1/0
          break;
        }
      }
      else if (rootStr == "watering_garden")
      {
        switch ((char)payload[0])
        {
        case '0':
          watering_garden(false);
          break;
        case '1':
          watering_garden(true);
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

void setup()
{
  delay(500);
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(POOL_PUMP, OUTPUT);
  pinMode(WATERING_TERRACE, OUTPUT);
  pinMode(WATERING_GARDEN, OUTPUT);
  pinMode(POOLWATER_VALVE, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);

  Serial.println();
  Serial.println("Garden control is started");
  String thisBoard = ARDUINO_BOARD;
  Serial.println(thisBoard);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  watering_terrace(false);
  watering_garden(false);
  poolwater_valve(false);
  pool_pump(false);

  Tasks.add<temperature>("temperature")
      ->setClient(&client)
      ->startFps(0.017); // /Minute

  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send(200, "text/plain", "Garden-Service"); });

  // Route definieren  code von ChatGPT
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Zeit holen
    struct tm timeinfo;
    getLocalTime(&timeinfo);
    char timeString[30];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);

    // Antwort mit Zeitstempel
    String message = "Garden-Service ";
    message += timeString;
    request->send(200, "text/plain", message);
  });

  ElegantOTA.begin(&server); // Start ElegantOTA
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);
  ElegantOTA.setAutoReboot(true);
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
    client.publish("outGarden", "{\"msg\":\"Reconnect: Pool pump and irrigation\"}");
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
  ElegantOTA.loop();
  Tasks.update();

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  if (rainfall.update())
  {
    client.publish("outGarden/rainSensor/trip", "{\"trip\":true}"); // Den Wert übergeben, oder eine '1' für einen Zähler
  }

  if (millis() - lastMillis >= 1000)
  {
    client.publish("outGarden/pool_pump/state", String(poolPump_state).c_str());
    client.publish("outGarden/watering_terrace/state", String(watering_terrace_state).c_str());
    client.publish("outGarden/watering_garden/state", String(watering_garden_state).c_str());
    client.publish("outGarden/poolwater_valve/state", String(poolwater_valve_state).c_str());

    digitalWrite(LED_BUILTIN, MAIN_LED_STATE);
    MAIN_LED_STATE = !MAIN_LED_STATE;
    lastMillis = millis();
  }
} /*--------------------------------------------------------------------------*/
