#ifndef ESP8266SOILMOISTURESENSOR_H
#define ESP8266SOILMOISTURESENSOR_H

/*
 * Enables printf debug messages
 */
//#define DEBUG

/**
 * Build-in LEDs
 * - blue LED on ESP-12E connected to GPIO0
 * - LED connected to LED_BUILTIN mounted on PCB
 * Reference: https://github.com/nodemcu/nodemcu-devkit-v1.0/blob/master/NODEMCU_DEVKIT_V1.0.PDF
 * Reference: https://components101.com/sites/default/files/component_datasheet/ESP12E%20Datasheet.pdf
 */
#define BLUE_LED_BUILTIN              LED_BUILTIN
#define PCB_LED_BUILTIN               GPIO2
#define LED_OFF                       HIGH
#define LED_ON                        LOW

/**
 * Push button marked as flash connected to GPIO0
 * Reference: See above
 */
#define FLASH_BUTTON                  0
#define BUTTON_PRESSED                LOW
#define BUTTON_RELEASED               HIGH

/**
 * Counter for button debouncing to enable calibration
 */
#define BUTTON_START_CALIBRATION_COUNTER   (unsigned int)40000
#define BUTTON_NEXT_CALIBRATION_COUNTER   (unsigned int)20000

/**
 * Time between to sensor measurements during calibration in milli seconds
 */
#define SENSOR_CALIBRATION_MEASUREMENTTIMER (unsigned long)(1*200)

/**
 * Maximum of sensor readings for calibration
 */
#define SENSOR_CALIBRATION_READINGS_MAX     (unsigned int)20

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
 * Time between to sensor measurements in milli seconds
 */
#define SENSOR_MEASUREMENTTIMER (unsigned long)(1*20*1000)

/*
 * LED blink and during startup (period in ms)
 */
#define STARTUP_BLINK_COUNT         (int)0xff
#define STARTUP_BLINK_PERIOD        (int)500

/*
 * Number of LED error blink and period in ms
 */
#define CALIBRATION_BLINK_PERIOD    (int)1000

/*
 * Number of LED error blink and period in ms
 */
#define ERROR_BLINK_READ_FAILED     (int)3
#define ERROR_BLINK_WRITE_FAILED    (int)4
#define ERROR_BLINK_PERIOD          (int)1000

#endif /* ESP8266SOILMOISTURESENSOR_H */
