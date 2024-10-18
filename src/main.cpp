#include <Arduino.h>
#include <TaskManager.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "interface.h"
#include "dht22.h"
#include "bmp180.h"
#include "secrets.h"
#include <SFE_BMP180.h>

const char *ssid = SID;
const char *password = PW;
const char *mqtt_server = MQTT;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

bmp180 *_bmp180;
dht22 *_dht22;

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
    // The topic includes a '/', we'll try to read the number of bottles from just after that
    topicStr.remove(0, topicStr.indexOf('/') + 1);
    /*
      löscht inPump/ so dass in topicStr nur noch Egon übrig bleibt
    */
    if (topicStr.indexOf('/') >= 0)
    {
      String rootStr = topicStr.substring(0, topicStr.indexOf('/'));
      Serial.println(rootStr);
      if (rootStr == "pool")
      {
        switch (payload[0])
        {
        case '0': // false
          // Pump off
          pool_pump(false);
          break;
        case '1': // true
          // Pump on
          pool_pump(true);
          break;
        default:
          // Warning !! Undefined payload or not 1/0
          break;
        }
      }
      else if (rootStr == "watering")
      {
        switch ((char)payload[0])
        {
        case '0': // false
          watering(false);
          break;
        case '1': // true
          watering(true);
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
  pinMode(POOL_SWT, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(15, OUTPUT);

  pinMode(WATERING_SWT, OUTPUT);
  // pinMode(WATERING_LED_GREEN, OUTPUT);
  // pinMode(WATERING_LED_RED, OUTPUT);

  digitalWrite(13, LOW);
  // digitalWrite(POOL_LED_RED, LOW);
  digitalWrite(15, LOW);

  // digitalWrite(WATERING_LED_GREEN, LOW);
  // digitalWrite(WATERING_LED_RED, LOW);
  digitalWrite(WATERING_SWT, LOW);

  Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");
  String thisBoard = ARDUINO_BOARD;
  Serial.println(thisBoard);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Tasks.add<dht22>()->startFps(1); // alle 1 sec
  Tasks.add<bmp180>()->startFps(1);
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
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
} /*--------------------------------------------------------------------------*/

void loop()
{
  static unsigned long lastMillis = millis();
  float temp;
  char result[10];

  Tasks.update();

  if (!client.connected())
  {
    reconnect();
  }
  client.loop(); 

  if (millis() - lastMillis >= 10000)

    {
      temp = _bmp180->getTemperature();
      dtostrf(temp, 10, 1, result);
      client.publish("outGarden/temperature", result);

      temp = _bmp180->getPressureSealevel();
      dtostrf(temp, 10, 1, result);
      client.publish("outGarden/pressure", result);

      temp = _dht22->getHumidity();
      dtostrf(temp, 10, 1, result);
      client.publish("outGarden/humidity", result);
      
      lastMillis = millis();

      // client.publish("outGarden/humidity", getHumidity());
      // client.publish("outGarden/pressure", getPressure());
      // client.publish("outGarden/climate", getClimate());
    }
} /*--------------------------------------------------------------------------*/
