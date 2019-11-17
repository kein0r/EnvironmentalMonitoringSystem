/**
 * Generic sensor using homie scheeme
 * MQTT scheeme is done by https://github.com/marvinroger/homie-esp8266
 * Flash software via OTA or Serial. To reset configuration hold FLASH
 * buttong for 5+ seconds. Open http://marvinroger.github.io/homie-esp8266/configurators/v2/
 * and connect to AP provided hardware
 */
#include <Arduino.h>
#include <Thing.h>
#include <WebThingAdapter.h>
#include <ESP8266WiFi.h>
#include "Esp8266TemperaturHumiditySensor.h"
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#ifdef DHT_TYPE
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#endif
#ifdef DS18B20_PIN
#include <OneWire.h>
#include <DallasTemperature.h>
#endif
#ifdef HTU21DF
#include "Adafruit_HTU21DF.h"
#endif

bool htu21dfAvailable = false;

#ifdef DHT_TYPE
DHT_Unified dht(DHT_PIN, DHT_TYPE);
#endif

#ifdef DS18B20_PIN
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(DS18B20_PIN);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
#endif

#ifdef HTU21DF
Adafruit_HTU21DF htu21df = Adafruit_HTU21DF();
#endif

WebThingAdapter* adapter;

const char* sensorTypes[] = {"TemperatureSensor", nullptr};
ThingDevice environmentalSensor("EnvSensor", "Temperature & Humidity Sensor", sensorTypes);
ThingProperty temperature("temperature", "Temperature", NUMBER, "TemperatureProperty");
ThingProperty humidity("humidity", "Humidity", NUMBER, nullptr);


void setup() {
  Serial.begin(115200);
  /* Connect to WiFi, wait until connection was established */
  WiFi.hostname(MQTT_CLIENTID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ");
  bool blink = true;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    digitalWrite(LED_BUILTIN, blink ? LOW : HIGH); // active low led
    Serial.print(".");
  }
  //Start mDNS with name esp8266
  MDNS.begin(MQTT_CLIENTID);

  // Start OTA
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  Serial.println(" connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /* Init Sensors */
#ifdef DHT_TYPE
  dht.begin();
#endif
#ifdef DS18B20_PIN
  sensors.begin();
#endif
#ifdef HTU21DF
  htu21dfAvailable = htu21df.begin();
#endif

  adapter = new WebThingAdapter("led-lamp", WiFi.localIP());

  temperature.unit = "°C";
  humidity.unit = "%";
  environmentalSensor.addProperty(&temperature);
  environmentalSensor.addProperty(&humidity);
  adapter->addDevice(&environmentalSensor);
  adapter->begin();
  Serial.println("HTTP server started");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print("/things/");
  Serial.println(environmentalSensor.id);

}

void loop() {
  static unsigned long nextSensorRun = 0;
  ThingPropertyValue value;

  ArduinoOTA.handle();
  adapter->update();

  if (millis() > nextSensorRun)
  {
    nextSensorRun = millis() + SENSOR_MEASUREMENTTIMER;

    char sensorString[20];
    int numTemperatureSensorReadings = 0;
    float temperatureSensorReadings = 0;
    int numHumiditySensorReadings = 0;
    float humiditySensorReadings = 0;
    #ifdef DS18B20_PIN
    sensors.requestTemperatures();
    if (sensors.getDS18Count() > 0)
    {
      numTemperatureSensorReadings++;
      temperatureSensorReadings += sensors.getTempCByIndex(DS18B20_INDEX);
    }
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
    /* Note: As long as we stay below minimum sample rate the sensor will not be
    * read again but just the values from the temperature reading above will
    * will be used. */
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println("Error reading humidity!");
    }
    else {
      numHumiditySensorReadings++;
      humiditySensorReadings += event.relative_humidity;
    }
    #endif
    #ifdef HTU21DF
    if (htu21dfAvailable == true) {
      temperatureSensorReadings += htu21df.readTemperature();
      numTemperatureSensorReadings++;
      humiditySensorReadings += htu21df.readHumidity();
      numHumiditySensorReadings++;

    }
    #endif
    /* Calculate average of temperature and humidity and send out */
    if (numTemperatureSensorReadings > 0)
    {
      /* Calculate averate of all temperature sensor readings */
      temperatureSensorReadings = temperatureSensorReadings/numTemperatureSensorReadings;
      value.number = temperatureSensorReadings;
      temperature.setValue(value);
    }
    if (numHumiditySensorReadings > 0) {
      humiditySensorReadings = humiditySensorReadings/numHumiditySensorReadings;
      value.number = humiditySensorReadings;
      humidity.setValue(value);
    }
  }
}
