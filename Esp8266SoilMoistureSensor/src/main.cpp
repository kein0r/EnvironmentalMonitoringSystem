/**
 *
 */
#include <Arduino.h>
#include "Esp8266SoilMoistureSensor.h"
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>             //Local WebServer used to serve the configuration portal
#include <ESPAsyncWiFiManager.h>           //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266mDNS.h>
#include <Thing.h>
#include <WebThingAdapter.h>
#include <Ticker.h>
#include "config.h"

bool otaActive = false;

uint statusBlinkerCount = 0;
Ticker statusBlinkTrigger;

AsyncWebServer server(80);
DNSServer dns;

AsyncWiFiManager wifiManager(&server, &dns);

WebThingAdapter* adapter;

const char* sensorTypes[] = {"MultiLevelSensor", nullptr};
ThingDevice environmentalSensor("SoilMoistureSensor", "Soil Moisture Sensor", sensorTypes);
ThingProperty moisture("moisture", "Moisture", NUMBER, "LevelProperty");

void statusBlink() {
  if (statusBlinkerCount) {
    if (statusBlinkerCount % 2) {
      digitalWrite(RED_LED_BUILTIN, LED_ON);
    }
    else {
      digitalWrite(RED_LED_BUILTIN, LED_OFF);
    }
    statusBlinkerCount--;
  }
  if (statusBlinkerCount == 0) {
    digitalWrite(RED_LED_BUILTIN, LED_OFF);
    statusBlinkTrigger.detach();
  }
}

void setStatusBlink(float frequency, uint count) {
  statusBlinkerCount = count;
  statusBlinkTrigger.attach(frequency, statusBlink);
}

void setup() {
  Serial.begin(115200);
  // Initialize LED and set it LED_OFF
  pinMode(RED_LED_BUILTIN, OUTPUT);
  digitalWrite(RED_LED_BUILTIN, LED_OFF);
  // Initialize the pushbutton pin as an input:
  //pinMode(buttonPin, INPUT_PULLUP);

  /* *************** Read Stored Config Values ************** */
  if (!readConfigValues())
  {
    /* Reset values if config values could not be read */
    setStatusBlink(ERROR_BLINK_FREQ, ERROR_BLINK_READ_FAILED);
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
  wifiManager.autoConnect(thingLocationName);

  /* read updated parameters */
  strcpy(thingLocationName, thingLocation.getValue());

  if (!writeConfigValues())
  {
    setStatusBlink(ERROR_BLINK_FREQ, ERROR_BLINK_WRITE_FAILED);
    Serial.printf("Writing configuration values failed. Resetting config values\n");
    wifiManager.resetSettings();
  }

  //Start mDNS with name esp8266
  MDNS.begin(thingLocationName);

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


  /* *************** IOT ************************************ */
  adapter = new WebThingAdapter("Soil Moisture Sensor", WiFi.localIP());

  moisture.unit = "%";
  moisture.readOnly = true;
  environmentalSensor.title = thingLocationName;
  environmentalSensor.addProperty(&moisture);
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

  if ((millis() > nextSensorRun) && (otaActive == false))
  {
    nextSensorRun = millis() + SENSOR_MEASUREMENTTIMER;
    int numMoistureSensorReadings = 0;
    float moistureSensorReadings = 0;

    moistureSensorReadings = map(analogRead(MOISTURESENSORE_PIN), 0, 200, 0, 100);
    numMoistureSensorReadings++;
    /* Calculate average of temperature and moisture and send out */
    if (numMoistureSensorReadings > 0)
    {
      /* Calculate averate of all temperature sensor readings */
      moistureSensorReadings = moistureSensorReadings/numMoistureSensorReadings;
      value.number = moistureSensorReadings;
      moisture.setValue(value);
      /* workaround to prevent only second thing being updated */
      adapter->update();
#ifdef DEBUG
      Serial.print("Num moisture readings: ");
      Serial.print(numMoistureSensorReadings);
      Serial.print(" value: ");
      Serial.println(value.number);
#endif
    }
  MDNS.update();
  } /* if ((millis() > nextSensorRun) && (otaActive == false)) */
}
