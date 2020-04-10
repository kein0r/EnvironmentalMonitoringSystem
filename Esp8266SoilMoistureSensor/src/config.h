#ifndef CONFIG_H
#define CONFIG_H

/**
 * All configuration related variables and definitions should go in
 * this file
 */

#define DEBUG

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

/**
 * Default min/max values without calibration done
 * Calibration is done by placing the dry sensor in air and then
 * putting covering it in water.
 */
#define CALIBRATION_AIR_MAX         (int)1024
#define CALIBRATION_WATER_MIN       (int)0
extern unsigned int calibrationAir;
extern unsigned int calibrationWater;

void saveConfigCallback();
bool readConfigValues();
bool writeConfigValues();

#endif
