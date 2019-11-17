#ifndef ESP8266TEMPERATURHUMIDITYSENSOR_H
#define ESP8266TEMPERATURHUMIDITYSENSOR_H

/**
 * Id to be sent to the broker to identify the client.
 * Note: Can be overwritten using build_flags
 */
#ifndef MQTT_CLIENTID
#define MQTT_CLIENTID   "MozillaTestClient"
#endif

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

#endif /* ESP8266TEMPERATURHUMIDITYSENSOR_H */
