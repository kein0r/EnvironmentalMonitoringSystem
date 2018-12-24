#include <Arduino.h>
#include "Esp8266TemperaturHumiditySensor_cfg.h"
#include <Homie.h>
#include <ESP8266mDNS.h>
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

HomieNode temperatureNode("temperature", "temperature");
HomieNode humidityNode("humidity", "humidity");
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

void setupHandler() {
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
 temperatureNode.setProperty("unit").send("°C");
 humidityNode.setProperty("unit").send("%");
}

void loopHandler() {
  static unsigned long nextSensorRun = 0;
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
      sprintf(sensorString, "%2.2f", temperatureSensorReadings);
      temperatureNode.setProperty("degree").send(sensorString);
    }
    if (numHumiditySensorReadings > 0) {
      humiditySensorReadings = humiditySensorReadings/numHumiditySensorReadings;
      sprintf(sensorString, "%2.2f", humiditySensorReadings);
      humidityNode.setProperty("percentage").send(sensorString);
    }
  }
}


void setup() {
  Serial.begin(115200);

  /* Connect to WiFi, wait until connection was established */
  Homie_setFirmware("TemperatureHumiditySensor", "1.0.0");
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

  temperatureNode.advertise("unit");
  temperatureNode.advertise("degree");
  humidityNode.advertise("unit");
  humidityNode.advertise("percentage");

  Homie.setup();
}

void loop()
{
  Homie.loop();
}
