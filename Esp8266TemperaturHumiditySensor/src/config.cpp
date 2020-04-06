/*
 * All functions related for fs (EEPROM) handling
 *
 * Note: This needs to be a .cpp file or else compilation won*'t work
 */

#include <FS.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "config.h"

char thingLocationName[MAX_SENSOR_LOCATION_LENGTH];

static bool shouldSaveConfig = false;

void saveConfigCallback() {
  shouldSaveConfig = true;
}

/*
 * Read configuration values from config.json file stored in SPIFFS.
 * Make sure that name of the variable and json element matches
 * @return true if SPIFFS can be mounted, file config.json exists and values
 * could be parsed successfully, else false
 */

bool readConfigValues() {

  bool retVal = false;

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      /* file exists, reading and loading */
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        /* Allocate a buffer to store contents of the file. */
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument jsonDocument(size);
        DeserializationError error = deserializeJson(jsonDocument, buf.get());
        if (error) {
          Serial.print("fdeserializeJson failed with code");
          Serial.println(error.c_str());
        } else {
          Serial.println("\nparsed json");
          serializeJsonPretty(jsonDocument, Serial);
          strcpy(thingLocationName, jsonDocument["thingLocationName"]);

          retVal = true;
        }
        configFile.close();
      }
    }
  } else {
    /* do nothing */
  }
  return retVal;
}

bool writeConfigValues(){
  bool retVal = true;
  /* save the custom parameters to FS */
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonDocument jsonDocument(JSONDOCUMENT_SIZE);
    jsonDocument["thingLocationName"] = thingLocationName;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
      retVal = false;
    }

    serializeJsonPretty(jsonDocument, Serial);
    serializeJson(jsonDocument, configFile);
    configFile.close();

    retVal = true;
  }
  return retVal;
}
