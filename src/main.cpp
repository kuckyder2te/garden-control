/*
File name: main.cpp
Author:    Stefan Scholz / Wilhel Kuckelsberg
Date:      2024.10.10
Project:   Garden Control
*/

#include <Arduino.h>
#include <TaskManager.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "..\lib\model.h"
#include "..\lib\interface.h"
#include "..\lib\secrets.h"
#include "..\lib\def.h"
#include "..\lib\rainfall.h"

// #include <ArduinoOTA.h>

const char *ssid = SID;
const char *password = PW;
const char *mqtt_server = MQTT;


WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
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
      else if (rootStr == "watering-valve")
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
       else if (rootStr == "poolwater-valve")
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
  delay(2000);
  Serial.begin(115200);

  pinMode(POOL_PUMP, OUTPUT);
  digitalWrite(POOL_PUMP, HIGH);

  pinMode(WATERING_VALVE, OUTPUT);
  digitalWrite(WATERING_VALVE, HIGH);

  pinMode(POOLWATER_VALVE, OUTPUT);
  digitalWrite(POOLWATER_VALVE, HIGH);

  Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");
  String thisBoard = ARDUINO_BOARD;
  Serial.println(thisBoard);

  setup_wifi();

  // ArduinoOTA.setHostname(ESPHostname);   //Rainsensor
  // ArduinoOTA.setPassword("admin");
  // ArduinoOTA.begin();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Tasks.add<rainFall>("RAIN")
  ->setModel(&model.rainMenge)
  ->startFps(0.1);

} /*--------------------------------------------------------------------------*/

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outGarden", "Garden control");
      // ... and resubscribe
      client.subscribe("inGarden/#");
      /*
      client.subscribed zu allen Nachrichten wie z.B.
      inPump/Status
      inPump/Egon
      inPump/Willy
*/
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
} /*--------------------------------------------------------------------------*/

void loop()
{
  static unsigned long lastMillis = millis();
  uint16_t elapsed_time = 10000; // 10 sec

  Tasks.update();

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  if (millis() - lastMillis >= elapsed_time)
  {
    client.publish("outGarden/pool-pump/state", String(model.interface.poolPump_state).c_str());
    client.publish("outGarden/watering-valve/state", String(model.interface.watering_valve_state).c_str());
    client.publish("outGarden/poolwater-valve/state", String(model.interface.poolwater_valve_state).c_str());
    lastMillis = millis();
  }
} /*--------------------------------------------------------------------------*/
