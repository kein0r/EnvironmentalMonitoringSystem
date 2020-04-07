#ifndef CONFIG_H
#define CONFIG_H

/**
 * Sizeof the json buffer during write. Not entirely sure how this is to be
 * calculated.
 */
#define JSONDOCUMENT_SIZE (int)200

/**
 * Max characters for sensor sensorLocation
 */
#define MAX_SENSOR_LOCATION_LENGTH    (int)40

extern char thingLocationName[MAX_SENSOR_LOCATION_LENGTH];

void saveConfigCallback();
bool readConfigValues();
bool writeConfigValues();

#endif
