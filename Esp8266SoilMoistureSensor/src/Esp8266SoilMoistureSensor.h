#ifndef ESP8266SOILMOISTURESENSOR_H
#define ESP8266SOILMOISTURESENSOR_H

/*
 * Enables printf debug messages
 */
#define DEBUG

/**
 * Build-in LEDs
 * - blue LED on ESP-12E connected to GPIO0
 * - LED connected to LED_BUILTIN mounted on PCB
 */
#define BLUE_LED_BUILTIN              LED_BUILTIN
#define PCB_LED_BUILTIN               GPIO2

/*
 * #defines in order to make code more readable
 */
#define LED_OFF                       HIGH
#define LED_ON                        LOW

/**
 * IP, Gateway and netmask for configuration mode
 */
#define WIFI_MANAGER_AP_IP            IPAddress(192,168,1,1)
#define WIFI_MANAGER_AP_GATEWAY       IPAddress(192,168,1,99)
#define WIFI_MANAGER_AP_NETMASK       IPAddress(255,255,255,0)

/**
 * Capacitive moisture sensore port
*/
#define MOISTURESENSORE_PIN           A0

/**
 * Default min/max values without calibration done
 * Calibration is done by placing the dry sensor in air and then
 * putting covering it in water.
 */
#define CALIBRATION_AIR_MAX         (int)1024
#define CALIBRATION_WATER_MIN       (int)0

/**
 */
#define SENSOR_MEASUREMENTTIMER (unsigned long)(1*1000)

/*
 * LED blink and during startup
 */
#define STARTUP_BLINK_COUNT         (int)0xff
#define STARTUP_BLINK_FREQ          (float)0.5

/*
 * Number of LED error blink and frequency, actualel freq/2
 */
#define CALIBRATION_LOWER_COUNT     (int)3
#define CALIBRATION_UPPER_COUNT     (int)4
#define CALIBRATION_BLINK_FREQ      (float)2.0

/*
 * Number of LED error blink and frequency, actualel freq/2
 */
#define ERROR_BLINK_READ_FAILED     (int)3
#define ERROR_BLINK_WRITE_FAILED    (int)4
#define ERROR_BLINK_FREQ            (float)1.0

#endif /* ESP8266SOILMOISTURESENSOR_H */
