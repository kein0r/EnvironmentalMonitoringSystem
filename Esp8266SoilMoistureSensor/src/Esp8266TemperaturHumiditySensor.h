#ifndef ESP8266TEMPERATURHUMIDITYSENSOR_H
#define ESP8266TEMPERATURHUMIDITYSENSOR_H

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
 * DHT sensor configuration
 * DHT_TYPE:
 * - DHT11
 * - DHT 21 (AM2301)
 * - DHT 22 (AM2302)
 * DHT_PIN: I/O pin which is connected to the DHT sensor
*/
#define DHT_TYPE        DHT22
#define DHT_PIN         2

/**
 * DS18B20 sensor configuration
 * I/O pin which is connected to the DHT sensor
 */
#define DS18B20_PIN     4
#define DS18B20_INDEX   0

/**
 * Adafruit HTU21D-F Temperature & Humidity Sensor
*/
#define HTU21DF

/**
 * Adafruit SI7021 Temperature & Humidity Sensor
*/
#define SI7021

/**
 */
#define SENSOR_MEASUREMENTTIMER (unsigned long)(1*30*1000)

/*
 * Number of LED error blink and frequency
 */
#define ERROR_BLINK_READ_FAILED     (int)2
#define ERROR_BLINK_WRITE_FAILED    (int)3
#define ERROR_BLINK_FREQ            (unsigned long)1000

#endif /* ESP8266TEMPERATURHUMIDITYSENSOR_H */
