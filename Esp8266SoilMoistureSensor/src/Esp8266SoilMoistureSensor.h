#ifndef ESP8266SOILMOISTURESENSOR_H
#define ESP8266SOILMOISTURESENSOR_H

/*
 * Enables printf debug messages
 */
#define DEBUG

/**
 * Build-in LEDs
 * - blue LED on ESP-12E connected to GPIO0
 * - red LED connected to LED_BUILTIN
 */
#define BLUE_LED_BUILTIN              GPIO2
#define RED_LED_BUILTIN               LED_BUILTIN

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
 */
#define SENSOR_MEASUREMENTTIMER (unsigned long)(1*30*1000)

/*
 * Number of LED error blink and frequency, actualel freq/2
 */
#define CALIBRATION_LOWER           (int)3
#define CALIBRATION_UPPER           (int)4
#define CALIBRATION_BLINK_FREQ      (float)2.0

/*
 * Number of LED error blink and frequency, actualel freq/2
 */
#define ERROR_BLINK_READ_FAILED     (int)3
#define ERROR_BLINK_WRITE_FAILED    (int)4
#define ERROR_BLINK_FREQ            (float)1.0

#endif /* ESP8266SOILMOISTURESENSOR_H */
