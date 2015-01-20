#include <ioCC2530.h>
#include <PlatformTypes.h>
#include <board.h>
#include <USART.h>
#include <IEEE_802.15.4.h>
#include <CC253x.h>
#include <string.h>
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

APIFrame_t rxAPIFrame;
APIFramePayload_t uartRxPayload[20];
APIFrame_t txAPIFrame;
APIFramePayload_t uartTxPayload[20];


void main( void )
{
  IEEE802154_PANIdentifier_t tempPanID;
  uint8_t UART_rxLength = 0;
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
  
  /* Prepare rx and tx UART frames */
  rxAPIFrame.data = uartRxPayload;
  txAPIFrame.data = uartTxPayload;
  UART_init();
  USART_setBaudrate(CC2530Bee_Config.USART_Baudrate);
  USART_setParity(CC2530Bee_Config.USART_Parity);
  
  IEEE802154_radioInit(&(CC2530Bee_Config.IEEE802154_config));
  enableAllInterrupt();
  
  sleepTime.value = 0xffff;
  
  /* now everyhting is set-up, start main loop now */
  while(1)
  {
    /* receive header via UART and convert length to little-endian */
    UART_rxLength = USART_read((char *)&(rxAPIFrame.header), sizeof(APIFrameHeader_t));
    /* received data is big-endian, mcu is little endian */
    SWAP_UINT16(rxAPIFrame.header.length);
    if ( (UART_rxLength == sizeof(APIFrameHeader_t)) && (rxAPIFrame.header.delimiter == UARTFrame_Delimiter) )
    {
      /* TODO: Check for frame length to be maximum of buffer */
      /* only proceed if CRC was ok */
      if (UARTAPI_receiveFrame(&rxAPIFrame) == UARTFrame_CRC_OK)
      {
        switch (rxAPIFrame.data[0])
          {
            case UARTAPI_ATCOMMAND:
              USART_writeline("Option 01 selected");
              break;
            case UARTAPI_ATCOMMAND_QUEUE:
              USART_writeline("Option 01 selected");
              break;
            case UARTAPI_REMOTE_AT_COMMAND_REQUEST:
              USART_writeline("Option 01 selected");
              break;
            case UARTAPI_TRAMSMIT_REQUEST_64BIT:
              CC2530Bee_Config.IEEE802154_TxDataFrame.sequenceNumber = rxAPIFrame.data[UARTAPI_64BITTRANSMIT_FRAMEID];
              memcpy(&(CC2530Bee_Config.IEEE802154_TxDataFrame.destinationAddress.extendedAdress), &(rxAPIFrame.data[UARTAPI_64BITTRANSMIT_ADDRESS]), sizeof(IEEE802154_ExtendedAddress_t) );
              /* save PAN ID in temporary variable in case it needs to be altered for this transmission */
              tempPanID = CC2530Bee_Config.IEEE802154_TxDataFrame.destinationPANID;
              if (rxAPIFrame.data[UARTAPI_64BITTRANSMIT_OPTIONS] & UARTAPI_TRANSMIT_OPTIONS_DISABLEACK) {
                CC2530Bee_Config.IEEE802154_TxDataFrame.fcf.ackRequired = 0;
              }
              if (rxAPIFrame.data[UARTAPI_64BITTRANSMIT_OPTIONS] & UARTAPI_TRANSMIT_OPTIONS_BROADCASTPANID) {
                CC2530Bee_Config.IEEE802154_TxDataFrame.destinationPANID = IEEE802154_BROADCAST_PAN_ID;
              }
              /* set correct address mode in fcf */
              CC2530Bee_Config.IEEE802154_TxDataFrame.fcf.destinationAddressMode = IEEE802154_FCF_ADDRESS_MODE_64BIT;
              CC2530Bee_Config.IEEE802154_TxDataFrame.fcf.sourceAddressMode = IEEE802154_FCF_ADDRESS_MODE_64BIT;
              /* point IEEE802154 payload pointer to data received via UART */
              CC2530Bee_Config.IEEE802154_TxDataFrame.payload = &(rxAPIFrame.data[UARTAPI_64BITTRANSMIT_DATA]);
              IEEE802154_radioSentDataFrame(&(CC2530Bee_Config.IEEE802154_TxDataFrame), rxAPIFrame.header.length - UARTAPI_64BITTRANSMIT_DATA);
              /* reset values back to "normal" which might have been changed above */
              CC2530Bee_Config.IEEE802154_TxDataFrame.destinationPANID = tempPanID;
              CC2530Bee_Config.IEEE802154_TxDataFrame.fcf.ackRequired = 1;
              break;
            case UARTAPI_TRAMSMIT_REQUEST_16BIT:
              CC2530Bee_Config.IEEE802154_TxDataFrame.sequenceNumber = rxAPIFrame.data[UARTAPI_16BITTRANSMIT_FRAMEID];
              CC2530Bee_Config.IEEE802154_TxDataFrame.destinationAddress.shortAddress = (IEEE802154_ShortAddress_t) rxAPIFrame.data[UARTAPI_16BITTRANSMIT_ADDRESS];
              /* save PAN ID in temporary variable in case it needs to be altered for this transmission */
              tempPanID = CC2530Bee_Config.IEEE802154_TxDataFrame.destinationPANID;
              if (rxAPIFrame.data[UARTAPI_16BITTRANSMIT_OPTIONS] & UARTAPI_TRANSMIT_OPTIONS_DISABLEACK) {
                CC2530Bee_Config.IEEE802154_TxDataFrame.fcf.ackRequired = 0;
              }
              if (rxAPIFrame.data[UARTAPI_16BITTRANSMIT_OPTIONS] & UARTAPI_TRANSMIT_OPTIONS_BROADCASTPANID) {
                CC2530Bee_Config.IEEE802154_TxDataFrame.destinationPANID = IEEE802154_BROADCAST_PAN_ID;
              }
              /* set correct address mode in fcf */
              CC2530Bee_Config.IEEE802154_TxDataFrame.fcf.destinationAddressMode = IEEE802154_FCF_ADDRESS_MODE_16BIT;
              CC2530Bee_Config.IEEE802154_TxDataFrame.fcf.sourceAddressMode = IEEE802154_FCF_ADDRESS_MODE_16BIT;
              /* point IEEE802154 payload pointer to data received via UART */
              CC2530Bee_Config.IEEE802154_TxDataFrame.payload = &(rxAPIFrame.data[UARTAPI_16BITTRANSMIT_DATA]);
              IEEE802154_radioSentDataFrame(&(CC2530Bee_Config.IEEE802154_TxDataFrame), rxAPIFrame.header.length - UARTAPI_16BITTRANSMIT_DATA);
              /* reset values back to "normal" which might have been changed above */
              CC2530Bee_Config.IEEE802154_TxDataFrame.destinationPANID = tempPanID;
              CC2530Bee_Config.IEEE802154_TxDataFrame.fcf.ackRequired = 1;
            break;
            /* no default as the frame will be silently discarded */
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
  config->USART_Baudrate = USART_Baudrate_9600;
  config->USART_Parity = USART_Parity_8BitNoParity;
  
  /* General radio configuration */
  config->IEEE802154_config.Channel = IEEE802154_Default_Channel;
  config->IEEE802154_config.PanID = IEEE802154_Default_PanID;
  config->IEEE802154_config.address.shortAddress = IEEE802154_Default_ShortAddress;
  
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
  config->IEEE802154_TxDataFrame.fcf.sourceAddressMode = IEEE802154_Default_SourceAdressingMode;
  /* preset variable to some meaningfull values */
  config->IEEE802154_TxDataFrame.sequenceNumber = 0x00;
  config->IEEE802154_TxDataFrame.destinationPANID = IEEE802154_Default_PanID;
  config->IEEE802154_TxDataFrame.destinationAddress.shortAddress = 0xffff;   /* broadcast */
  config->IEEE802154_TxDataFrame.sourceAddress.shortAddress = IEEE802154_Default_ShortAddress;
  
  config->RO_PacketizationTimeout = IEEE802154_Default_RO_PacketizationTimeout * 10;
  
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