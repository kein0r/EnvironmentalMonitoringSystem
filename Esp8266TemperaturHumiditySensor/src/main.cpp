#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#include "Esp8266TemperaturHumiditySensor_cfg.h"

const char* ssid = WIFI_SSID;
const char* password =  WIFI_PASSWORD;
const char* mqttServer = "m11.cloudmqtt.com";
const int mqttPort = 12948;

WiFiClient espClient;
PubSubClient client(espClient);

DHT_Unified dht(DHT_PIN, DHT_TYPE);

void reconnectMQTTBroker()
{
  Serial.print("Connecting to MQTT broker ... ")
  while (!client.connected())
  {
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected!");
    }
    else
    {
      Serial.print("failed! State: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  /* Connect to WiFi, wait until connection was established */
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /* Connect to mqtt broker */
  IPAddress server(10, 0, 0, 12);
  client.setServer(mqtt_server, 1883);
}

void loop() {
    // put your main code here, to run repeatedly:
}
