#ifndef CONFIG_H
#define CONFIG_H

/**
 * Max characters for sensor sensorLocation
 */
#define MAX_SENSOR_LOCATION_LENGTH    (int)40

extern char thingLocationName[MAX_SENSOR_LOCATION_LENGTH];

void saveConfigCallback();
bool readConfigValues();
bool writeConfigValues();

#endif
