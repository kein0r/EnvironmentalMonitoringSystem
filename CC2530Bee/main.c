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

APIFramePayload_t uartRxPayload[20];

APIFrame_t rxAPIFrame;

void main( void )
{
  uint8_t UART_rxLength = 0;
  char test[20];
  uint8_t led_status = 0;
  sleepTimer_t sleepTime;
  Board_init(); /* calls CC253x_Init */
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
  
  /* Prepare rx and tx UART frames */
  rxAPIFrame.data = uartRxPayload;
  
  /* now everyhting is set-up, start main loop now */
  while(1)
  {
    /* receive header via UART and convert length to little-endian */
    UART_rxLength = USART_read((char *)&(rxAPIFrame.header), sizeof(APIFrameHeader_t));
    SWAP_UINT16(rxAPIFrame.header.length);
    if ( (UART_rxLength == sizeof(APIFrameHeader_t)) && (rxAPIFrame.header.delimiter == UARTFrame_Delimiter) )
    {
      /* TODO: Check for frame length to be maximum of buffer */
      /* only proceed if CRC was ok */
      if (UARTAPI_receiveFrame(&rxAPIFrame) == UARTFrame_CRC_OK)
      {
        switch (rxAPIFrame.data[0])
          {
            case 0x01:
              USART_writeline("Option 01 selected");
              break;
            default:
              USART_writeline("Nothing selected");
          }
      }
      else {
        USART_writeline("ERROR (CRC)");
      }
      led_status = ~led_status;
    }
    else {
      USART_writeline("ERROR (Header)");
    }
    //test[4] = 0;
    //USART_write(test);
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

/**
 * Receives a frame via USART, un-escapes the data, copy the result to data
 * pointer of frame and calculates crc.
 * This function used blocking USART_getc function to read data from UART. Also 
 * make sure that enough space is be provided in frame->data pointer to receive
 * frame.
 * @param frame: UART API frame with pre-filled header
 * @return UARTFrame_CRC_OK if crc matched, UARTFrame_CRC_Not_OK else
 */
uint8_t UARTAPI_receiveFrame(APIFrame_t *frame)
{
  APIFramePayload_t *dataPtr = frame->data;
  uint8_t crc = 0;
  for (uint16_t i=0; i<frame->header.length; i++)
  {
    USART_getc((char *) dataPtr);
    /* if needed un-escape the data */
    if ( (*dataPtr == UARTFrame_Delimiter) || (*dataPtr == UARTFrame_Escape_Character) || (*dataPtr == UARTFrame_XON) || (*dataPtr == UARTFrame_XOFF) )
    {
      USART_getc((char *) dataPtr);
      *dataPtr ^= UARTFrame_Escape_Mask;
    }
    crc += *dataPtr;
    dataPtr++;
  }
  /* Check crc. The sum of received data + received crc must be 0xff */
  USART_getc((char *) &(frame->crc));
  crc += frame->crc;
  if (crc == 0xff)
  {
    return UARTFrame_CRC_OK;
  } 
  else
  {
    return UARTFrame_CRC_Not_OK;
  }
}