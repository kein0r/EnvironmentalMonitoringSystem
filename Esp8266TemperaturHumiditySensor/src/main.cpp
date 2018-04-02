#include <Arduino.h>
#include "Esp8266TemperaturHumiditySensor_cfg.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#ifdef DHT_TYPE
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#endif
#ifdef DS18B20_PIN
#include <OneWire.h>
#include <DallasTemperature.h>
#endif

WiFiClient espClient;
PubSubClient client(espClient);

#ifdef DHT_TYPE
DHT_Unified dht(DHT_PIN, DHT_TYPE);
#endif

#ifdef DS18B20_PIN
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(DS18B20_PIN);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
#endif

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
  WiFi.hostname(MQTT_CLIENTID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /* Announce this device via mDNS */
  if (!MDNS.begin(MQTT_CLIENTID)) {
    Serial.println("Error setting up MDNS responder!");
  }
  //MDNS.addService("esp", "tcp", 8080); // Announce esp tcp service on port 8080

  /* Connect to mqtt broker */
  client.setServer(MQTT_BROKERADDRESS, MQTT_BROKERPORT);
  client.setCallback(mqttCallback);

  /* Init Sensors */
#ifdef DHT_TYPE
  dht.begin();
#endif
#ifdef DS18B20_PIN
  sensors.begin();
#endif
}

void loop() {
    reconnectMQTTBroker();

    char sensorString[20];
    int numTemperatureSensorReadings = 0;
    float temperatureSensorReadings = 0;
#ifdef DS18B20_PIN
    sensors.requestTemperatures();
    numTemperatureSensorReadings++;
    temperatureSensorReadings += sensors.getTempCByIndex(DS18B20_INDEX);
#endif
#ifdef DHT_TYPE
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println("Error reading temperature!");
    }
    else {
      numTemperatureSensorReadings++;
      temperatureSensorReadings += event.temperature;
    }
#endif
    if (numTemperatureSensorReadings > 0)
    {
      /* Calculate averate of all temperature sensor readings */
      temperatureSensorReadings = temperatureSensorReadings/numTemperatureSensorReadings;
      sprintf(sensorString, "%2.2f", temperatureSensorReadings);
      client.publish(SENSOR_TEMPERATURETOPIC, sensorString);
    }
#ifdef DHT_TYPE
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
#endif
    delay(SENSOR_MEASUREMENTTIMER);
}
