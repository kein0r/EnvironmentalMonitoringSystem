#include <ioCC2530.h>
#include <PlatformTypes.h>
#include <board.h>
#include <USART.h>
#include <IEEE_802.15.4.h>
#include <CC253x.h>
#include "CC2530Bee.h"

/**
  * \brief No buffer is used to temporary store the received and sent frames.
  * For each frame that is sent or received a buffer must be allocated before
  * sending or receiving.
  * For this a message element (struct, array etc.) must be declared and allocated
  * to which the payload pointer of the IEE802154_header_t will point.
  */
IEE802154_Payload txPayload[30];
CC2530Bee_Config_t CC2530Bee_Config;

void main( void )
{
  char test[20];
  uint8_t led_status = 0;
  sleepTimer_t sleepTime;
  Board_init();
  /*P0DIR_0 = HAL_PINOUTPUT;
  P0DIR_2 = HAL_PINOUTPUT;
  P0DIR_3 = HAL_PINOUTPUT;
  P0DIR_4 = HAL_PINOUTPUT;*/
  P0DIR_4 = HAL_PININPUT;
  P0DIR_5 = HAL_PININPUT;

  CC2530Bee_loadConfig(&CC2530Bee_Config);
  ledInit();
  UART_init();
  USART_setBaudrate(CC2530Bee_Config.USART_Baudrate);
  USART_setParity(CC2530Bee_Config.USART_Parity);
  IEE802154_radioInit(&(CC2530Bee_Config.IEEE802154_config));
  enableAllInterrupt();
  
  sleepTime.value = 0xffff;
  
  /* now everyhting is set-up, start main loop now */
  while(1)
  {
    /* wait for reception */
    USART_read(test, 2);
    test[2] = 0;
    USART_write(test);
    led_status = ~led_status;
    P1_0 = led_status;
    /* ledOn();
    IEE802154_radioSentDataFrame(&sentFrameOne, sizeof(sensorInformation_t));
    ledOff();
    CC253x_IncrementSleepTimer(sleepTime);
    CC253x_ActivatePowerMode(SLEEPCMD_MODE_PM2);*/
  }
}

/**
 * Load config from EEPROM. If invalid CRC found default config is loaded
 */
void CC2530Bee_loadConfig(CC2530Bee_Config_t *config)
{
  config->USART_Baudrate = USART_Baudrate_57600;
  config->USART_Parity = USART_Parity_8BitNoParity;
  
  /* prepare header for IEEE 802.15.4 Tx message */
  config->IEEE802154_TxDataFrame.fcf.frameType = IEEE802154_FRAME_TYPE_DATA;  /* 3: 0x01 */
  config->IEEE802154_TxDataFrame.fcf.securityEnabled = IEEE802154_SECURITY_DISABLED; /* 1: 0x0 */
  config->IEEE802154_TxDataFrame.fcf.framePending = 0x0; /* 1:0x0 */
  config->IEEE802154_TxDataFrame.fcf.ackRequired = IEEE802154_ACKNOWLEDGE_REQUIRED; /* 1: 0x1 */
  config->IEEE802154_TxDataFrame.fcf.panIdCompression = IEEE802154_PANIDCOMPRESSION_DISABLED; /* 1: 0x0 */
  config->IEEE802154_TxDataFrame.fcf.destinationAddressMode = IEEE802154_ADDRESS_MODE_16BIT;
  config->IEEE802154_TxDataFrame.fcf.frameVersion = 0x00;
  config->IEEE802154_TxDataFrame.fcf.SourceAddressMode = IEEE802154_ADDRESS_MODE_16BIT;
  /* preset variable to some meaningfull values */
  config->IEEE802154_TxDataFrame.sequenceNumber = 0x00;
  config->IEEE802154_TxDataFrame.destinationPANID = 0xffff;
  config->IEEE802154_TxDataFrame.destinationAddress = 0xffff;
  config->IEEE802154_TxDataFrame.sourceAddress = 0xaffe;
  
}