#ifndef ESP8266SOILMOISTURESENSOR_H
#define ESP8266SOILMOISTURESENSOR_H

/*
 * Enables printf debug messages
 */
#define DEBUG

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
 * Number of LED error blink and frequency
 */
#define ERROR_BLINK_READ_FAILED     (int)2
#define ERROR_BLINK_WRITE_FAILED    (int)3
#define ERROR_BLINK_FREQ            (unsigned long)1000

#endif /* ESP8266SOILMOISTURESENSOR_H */
