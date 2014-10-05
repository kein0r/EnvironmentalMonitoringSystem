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
  * to which the payload pointer of the IEEE802154_header_t will point.
  */
IEEE802154_Payload txPayload[30];
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
  IEEE802154_radioInit(&(CC2530Bee_Config.IEEE802154_config));
  enableAllInterrupt();
  
  sleepTime.value = 0xffff;
  
  /* now everyhting is set-up, start main loop now */
  while(1)
  {
    /* wait for reception */
    USART_read(test, 4);
    test[4] = 0;
    USART_write(test);
    led_status = ~led_status;
    P1_0 = led_status;
    /* ledOn();
    IEEE802154_radioSentDataFrame(&sentFrameOne, sizeof(sensorInformation_t));
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
  
  /* General radio configuration */
  config->IEEE802154_config.Channel = IEEE802154_Default_Channel;
  config->IEEE802154_config.PanID = IEEE802154_Default_PanID;
  config->IEEE802154_config.ShortAddress = IEEE802154_Default_ShortAddress;
  
  /* prepare header for IEEE 802.15.4 Tx message */
  config->IEEE802154_TxDataFrame.fcf.frameType = IEEE802154_FCF_FRAME_TYPE_DATA;  /* 3: 0x01 */
  config->IEEE802154_TxDataFrame.fcf.securityEnabled = IEEE802154_FCF_SECURITY_DISABLED; /* 1: 0x0 */
  config->IEEE802154_TxDataFrame.fcf.framePending = 0x0; /* 1:0x0 */
  config->IEEE802154_TxDataFrame.fcf.ackRequired = IEEE802154_FCF_ACKNOWLEDGE_REQUIRED; /* 1: 0x1 */
#ifdef IEEE802154_ENABLE_PANID_COMPRESSION
  config->IEEE802154_TxDataFrame.fcf.panIdCompression = IEEE802154_FCF_PANIDCOMPRESSION_ENABLED; /* 1: 0x1 */
#else
  config->IEEE802154_TxDataFrame.fcf.panIdCompression = IEEE802154_FCF_PANIDCOMPRESSION_DISABLED; /* 1: 0x0 */
#endif
  
  config->IEEE802154_TxDataFrame.fcf.destinationAddressMode = IEEE802154_Default_DestinationAdressingMode;
  config->IEEE802154_TxDataFrame.fcf.frameVersion = 0x00;
  config->IEEE802154_TxDataFrame.fcf.SourceAddressMode = IEEE802154_Default_SourceAdressingMode;
  /* preset variable to some meaningfull values */
  config->IEEE802154_TxDataFrame.sequenceNumber = 0x00;
  config->IEEE802154_TxDataFrame.destinationPANID = IEEE802154_Default_PanID;
  config->IEEE802154_TxDataFrame.destinationAddress = 0xffff;   /* broadcast */
  config->IEEE802154_TxDataFrame.sourceAddress = IEEE802154_Default_ShortAddress;
  
  config->RO_PacketizationTimeout = IEEE802154_Default_RO_PacketizationTimeout * 1000;
  
}