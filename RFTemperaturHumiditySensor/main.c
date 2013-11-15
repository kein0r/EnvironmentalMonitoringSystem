#include <ioCC2530.h>
#include "PlatformTypes.h"
#include <board.h>
#include <dht22.h>
#include <IEEE_802.15.4.h>

/**
  * \brief No buffer is used to temporary store the received and sent frames.
  * For each frame that is sent or received a buffer must be allocated before
  * sending or receiving.
  * For this a message element (struct, array etc.) must be declared and allocated
  * to which the payload pointer of the IEE802154_header_t will point.
  */
typedef struct {
  uint8 id;                     /*!< message id must always come first */
  sint16 dht22Temperatur;       /*!< temperatur of dht22 */
  uint16 dht22RelativeHumidity; /*!< relative humidity of dht22 */
} sensorInformation_t;

sensorInformation_t sensorInformation;
IEE802154_DataFrameHeader_t sentFrameOne = {{0,0,0,0,0,0,0,0,0} ,0 ,0 ,0 ,0, (uint8*)&sensorInformation};

int main( void )
{
  volatile DHT22State_t DHT22State;
  Board_init();
  P0DIR_0 = HAL_PINOUTPUT;
  P0DIR_2 = HAL_PINOUTPUT;
  P0DIR_4 = HAL_PINOUTPUT;
  ledInit();
  DHT22_init();
  IEE802154_radioInit();
  
  /* prepare header for message */
  sentFrameOne.fcf.frameType = IEEE802154_FRAME_TYPE_DATA;  /* 3: 0x01 */
  sentFrameOne.fcf.securityEnabled = IEEE802154_SECURITY_DISABLED; /* 1: 0x0 */
  sentFrameOne.fcf.framePending = 0x0; /* 1:0x0 */
  sentFrameOne.fcf.ackRequired = IEEE802154_ACKNOWLEDGE_REQUIRED; /* 1: 0x1 */
  sentFrameOne.fcf.panIdCompression = IEEE802154_PANIDCOMPRESSION_DISABLED; /* 1: 0x0 */
  sentFrameOne.fcf.destinationAddressMode = IEEE802154_ADDRESS_MODE_16BIT;
  sentFrameOne.fcf.frameVersion = 0x00;
  sentFrameOne.fcf.SourceAddressMode = IEEE802154_ADDRESS_MODE_16BIT;
  sentFrameOne.sequenceNumber = 0x00;
  sentFrameOne.destinationPANID = 0xffff;
  sentFrameOne.destinationAddress = 0xffff;
  sentFrameOne.sourceAddress = 0xaffe;
  sensorInformation.id = 0x42;

  while(1)
  {
    ledOn();
    DHT22State = DHT22_readValues();
    /* prepare values */
    sensorInformation.dht22Temperatur = DHT22_SensorValue.values.Temperatur;
    sensorInformation.dht22RelativeHumidity = DHT22_SensorValue.values.RelativeHumidity;
    IEE802154_radioSentDataFrame(&sentFrameOne, sizeof(sensorInformation_t));
    ledOff();
    delay_ms(1000);
  }
  return 0;
}
