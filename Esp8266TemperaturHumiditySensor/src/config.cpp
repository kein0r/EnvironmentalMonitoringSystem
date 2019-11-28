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
 * @return true if SPIFS can be mounted, file config.json exists and values
 * could be parsed successfully, else false
 */

bool readConfigValues() {

  bool retVal = false;

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(thingLocationName, json["thingLocationName"]);

          retVal = true;
        } else {
          Serial.println("failed to load json config");
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
  bool retVal = false;
  return retVal;
}
