#include <ioCC2530.h>
#include "PlatformTypes.h"
#include <board.h>
#include <USART.h>
#include <IEEE_802.15.4.h>
#include <CC253x.h>

/**
  * \brief No buffer is used to temporary store the received and sent frames.
  * For each frame that is sent or received a buffer must be allocated before
  * sending or receiving.
  * For this a message element (struct, array etc.) must be declared and allocated
  * to which the payload pointer of the IEE802154_header_t will point.
  */
typedef struct {
  uint8 id;                     /*!< message id must always come first */
  sint16 test;       /*!< temperatur of dht22 */
  uint16 test2; /*!< relative humidity of dht22 */
} sensorInformation_t;

sensorInformation_t sensorInformation;
IEE802154_DataFrameHeader_t sentFrameOne = {{0,0,0,0,0,0,0,0,0} ,0 ,0 ,0 ,0, (uint8*)&sensorInformation};

void main( void )
{
  sleepTimer_t sleepTime;
  Board_init();
  P0DIR_0 = HAL_PINOUTPUT;
  P0DIR_2 = HAL_PINOUTPUT;
  P0DIR_4 = HAL_PINOUTPUT;
  ledInit();
  USART_init(USART_BAUDRATE_DEFAULT);
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

  sleepTime.value = 0xffff;
  while(1)
  {
    ledOn();
    /* prepare values */
    IEE802154_radioSentDataFrame(&sentFrameOne, sizeof(sensorInformation_t));
    ledOff();
    CC253x_IncrementSleepTimer(sleepTime);
    CC253x_ActivatePowerMode(SLEEPCMD_MODE_PM2);
  }
}
