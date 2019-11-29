/**
 *
 */
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>             //Local WebServer used to serve the configuration portal
#include <ESPAsyncWiFiManager.h>           //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266mDNS.h>
#include <Thing.h>
#include <WebThingAdapter.h>

#include "Esp8266TemperaturHumiditySensor.h"
#include "config.h"

#ifdef DHT_TYPE
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#endif
#ifdef DS18B20_PIN
#include <OneWire.h>
#include <DallasTemperature.h>
#endif
#ifdef HTU21DF
#include <Adafruit_HTU21DF.h>
#endif
#ifdef SI7021
#include <Adafruit_Si7021.h>
#endif

bool otaActive = false;

int errorBlink = 0;

AsyncWebServer server(80);
DNSServer dns;

AsyncWiFiManager wifiManager(&server, &dns);

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
bool htu21dfAvailable = false;
Adafruit_HTU21DF htu21df = Adafruit_HTU21DF();
#endif

#ifdef SI7021
Adafruit_Si7021 si7021 = Adafruit_Si7021();
#endif

WebThingAdapter* adapter;

const char* sensorTypes[] = {"TemperatureSensor", nullptr};
ThingDevice environmentalSensor("EnvSensor", "Temperature & Humidity Sensor", sensorTypes);
ThingProperty temperature("temperature", "Temperature", NUMBER, "TemperatureProperty");
ThingProperty humidity("humidity", "Humidity", NUMBER, nullptr);


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  /* *************** Read Stored Config Values ************** */
  if (!readConfigValues())
  {
    /* Reset values if config values could not be read */
    errorBlink += ERROR_BLINK_READ_FAILED;
    Serial.printf("Reading configuration values failed. Resetting config values\n");
    wifiManager.resetSettings();
  }

  /* *************** AsyncWifiManager *********************** */
  AsyncWiFiManagerParameter thingParameter("<p>Thing Parameter</p>");
  wifiManager.addParameter(&thingParameter);
  wifiManager.addParameter(new AsyncWiFiManagerParameter("Location and Description</p>"));
  AsyncWiFiManagerParameter thingLocation("thingLocation", "Thing Location/Description", thingLocationName, MAX_SENSOR_LOCATION_LENGTH);
  wifiManager.addParameter(&thingLocation);

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.setAPStaticIPConfig(WIFI_MANAGER_AP_IP, WIFI_MANAGER_AP_GATEWAY, WIFI_MANAGER_AP_NETMASK);
  //

  /* Connect to WiFi, wait until connection was established or configuration done */
  Serial.print("Connecting to WiFi ");
  if (!wifiManager.autoConnect("AutoConnectAP")) {
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
  }
  wifiManager.autoConnect(IOT_CLIENTID);

  /* read updated parameters */
  strcpy(thingLocationName, thingLocation.getValue());

  if (!writeConfigValues())
  {
    errorBlink += ERROR_BLINK_WRITE_FAILED;
    Serial.printf("Writing configuration values failed. Resetting config values\n");
    wifiManager.resetSettings();
  }

  //Start mDNS with name esp8266
  // TODO: check if Wifi.hostname is needed
  MDNS.begin(IOT_CLIENTID);

  /* *************** OTA ************************************ */
  // Start OTA
  ArduinoOTA.onStart([]() {
    otaActive = true;
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    otaActive = false;
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

  /* *************** Sensors ******************************** */

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


  /* *************** IOT ************************************ */
  adapter = new WebThingAdapter("Temperature Humidity Sensor", WiFi.localIP());

  temperature.unit = "°C";
  humidity.unit = "%";
  environmentalSensor.id = thingLocationName;
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
  static unsigned long nextErrorBlink = 0;
  static int errorBlinkState = LOW;

  ThingPropertyValue value;

  ArduinoOTA.handle();
  adapter->update();

  if ((millis() > nextSensorRun) && (otaActive == false))
  {
    nextSensorRun = millis() + SENSOR_MEASUREMENTTIMER;

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
  } /* if ((millis() > nextSensorRun) && (otaActive == false)) */

  if (millis() > nextErrorBlink)
  {
    nextErrorBlink = millis() + ERROR_BLINK_FREQ/2;
    if (errorBlinkState == LOW)
    {
      /* LED is off, turn on if needed */
      if (errorBlink > 0)
      {
        errorBlinkState = HIGH;
        digitalWrite(LED_BUILTIN, errorBlinkState);
        errorBlink--;
      }
    }
    if (errorBlinkState == HIGH)
    {
      /* LED was on, turn it off and decrement counter */
      errorBlinkState = LOW;
      digitalWrite(LED_BUILTIN, errorBlinkState);
    }
  } /* if ((millis() > nextSensorRun) */
}
