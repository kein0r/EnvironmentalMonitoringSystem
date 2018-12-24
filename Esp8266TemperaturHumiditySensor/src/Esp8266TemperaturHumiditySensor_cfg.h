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
