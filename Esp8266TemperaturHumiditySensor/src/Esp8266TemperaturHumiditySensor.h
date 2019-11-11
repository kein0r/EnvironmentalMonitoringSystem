#ifndef ESP8266TEMPERATURHUMIDITYSENSOR_H
#define ESP8266TEMPERATURHUMIDITYSENSOR_H

/**
 * Wifi related configuration
 * Only set if they're not already set in platformio.ini using
 * build_flags = -DWIFI_SSID=\"AnotherTestWifi\" -DWIFI_PASSWORD=\"NewPassword\"
*/
#ifndef WIFI_SSID
#define WIFI_SSID       "TestWifi"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD   "TestWifiPassword"
#endif

/**
 * MQTT_CLIENTID. Id to be sent to the broker to identify the client.
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
 */
#define SENSOR_MEASUREMENTTIMER (unsigned long)(2*60*1000)

#endif /* ESP8266TEMPERATURHUMIDITYSENSOR_H */
