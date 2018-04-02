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
#define MQTT_CLIENTID   "ESP8266Client"
#endif

/**
 * MQTT configuration
 * MQTT_BROKERADDRESS either a string "whatever.mqttbrokr.com" or an ip IPAddress
 * IPAddress(192, 168, 0, 55)
 * MQTT_BROKERPORT port used to connect to
 * MQTT_RECONNECTTIMER time between two connection attempts in case connection
 * can't be estatblished or is lost
 */
#define MQTT_BROKERADDRESS      IPAddress(10, 0, 0, 12)
#define MQTT_BROKERPORT         (int)1883
#define MQTT_RECONNECTTIMER     (unsigned long)5000

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
 * Topics used by this sensor (all lower case)
 * SENSOR_HUMIDITYTOPIC and SENSOR_TEMPERATURETOPIC humidity and temperature to
 * be send to the MQTT broker
 * SENSOR_LEDTOPIC to subscribe to, to toggle the LED
 */
#define SENSOR_TEMPERATURETOPIC "home/groundfloor/diningroom/temperature"
#define SENSOR_HUMIDITYTOPIC    "home/groundfloor/diningroom/humidity"
#define SENSOR_LEDTOPIC         "home/groundfloor/diningroom/led"
#define SENSOR_MEASUREMENTTIMER (unsigned long)(2*60*1000)

/**
 *
 */
#define ANNOUNCE_ADDRESSTOPIC   "home/clients/address"
#define ANNOUNCE_TOPICSTOPIC    "home/clients/topics"
