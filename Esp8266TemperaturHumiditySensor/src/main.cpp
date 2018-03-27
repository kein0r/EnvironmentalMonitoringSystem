#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Esp8266TemperaturHumiditySensor_cfg.h"

WiFiClient espClient;
PubSubClient client(espClient);

DHT_Unified dht(DHT_PIN, DHT_TYPE);

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(4);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

/**
 * Connect or re-connect to mqtt broker.
 * Note: This function is blocking! It will wait until a connection to the
 * mqtt broker can be established.
 */
void reconnectMQTTBroker()
{
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT broker ... ");
    if (client.connect(MQTT_CLIENTID))
    {
      /* Publish some information about the module */
      client.publish(ANNOUNCE_ADDRESSTOPIC, WiFi.localIP().toString().c_str());
      Serial.println("connected!");
    }
    else
    {
      Serial.print("failed! State: ");
      Serial.print(client.state());
      delay(MQTT_RECONNECTTIMER);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  /* Connect to WiFi, wait until connection was established */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /* Connect to mqtt broker */
  client.setServer(MQTT_BROKERADDRESS, MQTT_BROKERPORT);
  client.setCallback(mqttCallback);

  /* Init Sensors */
  dht.begin();
  sensors.begin();
}

void loop() {
    reconnectMQTTBroker();

    sensors_event_t event;
    char sensorString[20];
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println("Error reading temperature!");
    }
    else {
      sprintf(sensorString, "%2.2f", event.temperature);
      client.publish(SENSOR_TEMPERATURETOPIC, sensorString);
    }
    /* Note: As long as we stay below minimum sample rate the sensor will not be
     * read again but just the values from the temperature reading above will
     * will be used. */
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println("Error reading humidity!");
    }
    else {
      sprintf(sensorString, "%2.2f", event.relative_humidity);
      client.publish(SENSOR_HUMIDITYTOPIC, sensorString);
    }
    sensors.requestTemperatures();
    sprintf(sensorString, "%2.2f", sensors.getTempCByIndex(0));
    client.publish(SENSOR_TEMPERATURETOPIC, sensorString);
    delay(SENSOR_MEASUREMENTTIMER);
}
