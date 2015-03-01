/** @ingroup CC2530Bee
 * @{
 */
#include <ioCC2530.h>
#include <PlatformTypes.h>
#include <board.h>
#include <USART.h>
#include <WatchdogTimer.h>
#include <IEEE_802.15.4.h>
#include <CC253x.h>
#include <string.h>
#include "CC2530Bee.h"

/**
 * \mainpage CC2530Bee
 * @brief CC2530 based implementation of famous XBee devices from Digi International Inc (see http://www.digi.com)
 *
 * The firmware for CC2530 was implemented to be able to use this chip as a drop-in replacement for the famous XBee devices together
 * with Arduino XBee library (see https://code.google.com/p/xbee-arduino/).
 * This implementation only supports "API Operation" using escape characters called "AP = 2: API Operation (with escaped characters)" in
 * XBee documentation. The reason is that this is the only mode supported by the Arduino library.
 * A simple python script is provided to test the functionality. Rx tests will only works with SmartRF Studio.
 * 
 * Implemented API Commands
 * ========================
 * - AT Command: API Identifier Value: 0x08. Partial, test for implemented parts exists. Not all AT commands are supported (see list below which)
 * - AT Command Response: API Identifier Value: 0x88. Fully implemented, test exists
 * - TX (Transmit) Request: 64-bit address: API Identifier Value: 0x00. Fully implemented, test exists
 * - TX (Transmit) Request: 16-bit address: API Identifier Value: 0x01. Fully implemented, test exists
 * - TX (Transmit) Status: API Identifier Value: 0x89. Fully implemented, test exists (not all return values can be tested)
 * - RX (Receive) Packet: 64-bit Address: API Identifier Value: 0x80. Fully implemented, test exists
 * - RX (Receive) Packet: 16-bit Address: API Identifier Value: 0x81. Fully implemented, test exists

 * Functionality missing:
 * ========================
 * - AT Command - Queue Parameter Value: API Identifier Value: 0x09
 * - Remote AT Command Request: API Identifier Value: 0x17
 * - Remote Command Response: API Identifier Value: 0x97
 * - All kind of nonvolatile storage of parametes 

 * Supported AT commands:
 * ========================
 * - Software Rese FR: 0x4652
 * - Channel CH: 0x4848
 * - PanID ID: 0x4944
 * - Destination Address High DH: 0x4448
 * - Destination Address Low DL: 0x444c
 * - Source Address 16Bit MY: 0x 4d59
 * - Serialnumber High SH: 0x5348
 * - Serialnumber Low SL: 0x534c
*/

/**
  * General configuration struct
*/
CC2530Bee_Config_t CC2530Bee_Config;

/**
  * For each frame that is sent or received a buffer must be allocated before
  * sending or receiving.
  * For this a message element (struct, array etc.) must be declared and allocated
  * to which the payload pointer of the IEEE802154_header_t will point.
  */
IEEE802154_DataFrameHeader_t  IEEE802154_TxDataFrame;
IEEE802154_DataFrameHeader_t  IEEE802154_RxDataFrame;
IEEE802154_Payload radioRxPayload[100];

APIFrame_t rxAPIFrame;
APIFramePayload_t uartRxPayload[100];
APIFrame_t txAPIFrame;
APIFramePayload_t uartTxPayload[100];

/**
 * State of main state machine
*/
CC2530BeeState_t CC2530BeeState = CC2530BeeState_Normal;

void main( void )
{
  uint16_t atCommand;
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
  
  /* Prepare rx buffer for IEEE 802.15.4 */
  IEEE802154_RxDataFrame.payload = radioRxPayload;
  IEEE802154_radioInit(&(CC2530Bee_Config.IEEE802154_config));
  /* Tx source address is preloaded with chip's own 64bit address. Check if it should be used. */
  if (IEEE802154_TxDataFrame.sourceAddress.shortAddress == CC2530BEE_USE_64BIT_ADDRESSING)
  {
    IEEE802154_TxDataFrame.fcf.sourceAddressMode = IEEE802154_FCF_ADDRESS_MODE_64BIT;
  }
  else {
    IEEE802154_TxDataFrame.fcf.sourceAddressMode = IEEE802154_FCF_ADDRESS_MODE_16BIT;
  }
  enableAllInterrupt();
  
  //sleepTime.value = 0xffff;
  
  /* Check for reset reason and report via USART */
  txAPIFrame.data[0] = UARTAPI_MODEMSTATUS;
  if ((SLEEPSTA & SLEEPSTA_RST_MASK) == SLEEPSTA_RST_EXTERNALRESET)
  {
    txAPIFrame.data[UARTAPI_MODEMSTATUS_DATA] = UARTAPI_MODEMSTATUS_HARDWARE_RESET;
   UARTAPI_sentFrame(txAPIFrame.data, UARTAPI_MODEMSTATUS_LENGTH); 
  }
  if ((SLEEPSTA & SLEEPSTA_RST_MASK) == SLEEPSTA_RST_WATCHDOGRESET)
  {
    txAPIFrame.data[UARTAPI_MODEMSTATUS_DATA] = UARTAPI_MODEMSTATUS_WATCHDOG_RESET;
    UARTAPI_sentFrame(txAPIFrame.data, UARTAPI_MODEMSTATUS_LENGTH);
  }
  
  /* Enable watchdog to 250ms */
  WDT_init(WDT_INT_CLOCKTIMES8192);
  
  /* now everyhting is set-up, start main loop now */
  while(1)
  {
    WDT_trigger();
    /* Analyze current state, if in state CC2530BeeState_Normal read from UART */
    if (CC2530BeeState == CC2530BeeState_Reset)
    {
    //SRCRC.FORCE_RESET
    }
    /* if enough bytes in Rx buffer parse it */
    if (USART_numBytesInRxBuffer() >= sizeof(APIFrameHeader_t))
    {
      /* receive header via UART and convert length to little-endian */
      USART_read((char *)&(rxAPIFrame.header), sizeof(APIFrameHeader_t));
      /* received data is big-endian, mcu is little endian */
      SWAP_UINT16(rxAPIFrame.header.length);
      /* @todo Check for frame length to be maximum of buffer */
      /* only proceed if CRC was ok */
      if ((rxAPIFrame.header.delimiter == UARTFrame_Delimiter) && UARTAPI_receiveFrame(&rxAPIFrame) == UARTFrame_CRC_OK)
      {
        switch (rxAPIFrame.data[0])
          {
            case UARTAPI_ATCOMMAND:
              if (rxAPIFrame.header.length == UARTAPI_ATCOMMAND_READ_LENGTH)
              {
                UARTAPI_readParameter(rxAPIFrame.data);
              }
              else {
                UARTAPI_setParameter(rxAPIFrame.data);
              }
              break;
            case UARTAPI_ATCOMMAND_QUEUE:
              break;
            case UARTAPI_REMOTE_AT_COMMAND_REQUEST:
              USART_writeline("Option 01 selected");
              break;
            case UARTAPI_TRAMSMIT_REQUEST_64BIT:
              IEEE802154_TxDataFrame.sequenceNumber = rxAPIFrame.data[UARTAPI_64BITTRANSMIT_FRAMEID];
              memcpy(&(IEEE802154_TxDataFrame.destinationAddress.extendedAdress), &(rxAPIFrame.data[UARTAPI_64BITTRANSMIT_ADDRESS]), sizeof(IEEE802154_ExtendedAddress_t) );
              /* save PAN ID in temporary variable in case it needs to be altered for this transmission */
              tempPanID = IEEE802154_TxDataFrame.destinationPANID;
              if (rxAPIFrame.data[UARTAPI_64BITTRANSMIT_OPTIONS] & UARTAPI_TRANSMIT_OPTIONS_DISABLEACK) {
                IEEE802154_TxDataFrame.fcf.ackRequired = 0;
              }
              if (rxAPIFrame.data[UARTAPI_64BITTRANSMIT_OPTIONS] & UARTAPI_TRANSMIT_OPTIONS_BROADCASTPANID) {
                IEEE802154_TxDataFrame.destinationPANID = IEEE802154_BROADCAST_PAN_ID;
              }
              /* set correct address mode in fcf for destination address. The corresponding bit for source address will 
               * be set whenever source address is changed */
              IEEE802154_TxDataFrame.fcf.destinationAddressMode = IEEE802154_FCF_ADDRESS_MODE_64BIT;
              /* point IEEE802154 payload pointer to data received via UART */
              IEEE802154_TxDataFrame.payload = &(rxAPIFrame.data[UARTAPI_64BITTRANSMIT_DATA]);
              IEEE802154_radioSentDataFrame(&(IEEE802154_TxDataFrame), rxAPIFrame.header.length - UARTAPI_64BITTRANSMIT_DATA);
              /* reset values back to "normal" which might have been changed above */
              IEEE802154_TxDataFrame.destinationPANID = tempPanID;
              IEEE802154_TxDataFrame.fcf.ackRequired = 1;
              break;
            case UARTAPI_TRAMSMIT_REQUEST_16BIT:
              IEEE802154_TxDataFrame.sequenceNumber = rxAPIFrame.data[UARTAPI_16BITTRANSMIT_FRAMEID];
              IEEE802154_TxDataFrame.destinationAddress.shortAddress = *((IEEE802154_ShortAddress_t*)&rxAPIFrame.data[UARTAPI_16BITTRANSMIT_ADDRESS]);
              /* save PAN ID in temporary variable in case it needs to be altered for this transmission */
              tempPanID = IEEE802154_TxDataFrame.destinationPANID;
              if (rxAPIFrame.data[UARTAPI_16BITTRANSMIT_OPTIONS] & UARTAPI_TRANSMIT_OPTIONS_DISABLEACK) {
                IEEE802154_TxDataFrame.fcf.ackRequired = 0;
              }
              if (rxAPIFrame.data[UARTAPI_16BITTRANSMIT_OPTIONS] & UARTAPI_TRANSMIT_OPTIONS_BROADCASTPANID) {
                IEEE802154_TxDataFrame.destinationPANID = IEEE802154_BROADCAST_PAN_ID;
              }
              /* set correct address mode in fcf for destination address. The corresponding bit for source address will 
               * be set whenever source address is changed */
              IEEE802154_TxDataFrame.fcf.destinationAddressMode = IEEE802154_FCF_ADDRESS_MODE_16BIT;
              /* point IEEE802154 payload pointer to data received via UART */
              IEEE802154_TxDataFrame.payload = &(rxAPIFrame.data[UARTAPI_16BITTRANSMIT_DATA]);
              IEEE802154_radioSentDataFrame(&(IEEE802154_TxDataFrame), rxAPIFrame.header.length - UARTAPI_16BITTRANSMIT_DATA);
              /* reset values back to "normal" which might have been changed above */
              IEEE802154_TxDataFrame.destinationPANID = tempPanID;
              IEEE802154_TxDataFrame.fcf.ackRequired = 1;
              break;
            case UARTAPI_ECHOTEST:
              /* Service only implemented for USART testing 
               * Will sent every valid frame back exactly as it was received */
              UARTAPI_sentFrame(uartRxPayload, rxAPIFrame.header.length);
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
      /* noting */
    }
    /* ledOn();
    IEEE802154_radioSentDataFrame(&sentFrameOne, sizeof(sensorInformation_t));
    ledOff();
    CC253x_IncrementSleepTimer(sleepTime);
    CC253x_ActivatePowerMode(SLEEPCMD_MODE_PM2);*/
  }  // while(1)
}

/**
 * Load config from EEPROM. If invalid CRC found default config is loaded
 * @param config Pointer to configuration struct in which to store the configuration read from EEPROM
 * @todo Implement EEPROM stuff
 */
void CC2530Bee_loadConfig(CC2530Bee_Config_t *config)
{
  config->USART_Baudrate = USART_Baudrate_57600;
  config->USART_Parity = USART_Parity_8BitNoParity;
  
  /* General radio configuration */
  config->IEEE802154_config.Channel = CC2530BEE_Default_Channel;
  config->IEEE802154_config.PanID = CC2530BEE_Default_PanID;
  config->IEEE802154_config.shortAddress = CC2530BEE_Default_ShortAddress;
  
  /* prepare header for IEEE 802.15.4 Tx message. Values are stored in config but must be copied 
   * to #IEEE802154_TxDataFrame in order to be effective. */
  IEEE802154_TxDataFrame.fcf.frameType = IEEE802154_FCF_FRAME_TYPE_DATA;  /* 3: 0x01 */
  IEEE802154_TxDataFrame.fcf.securityEnabled = IEEE802154_FCF_SECURITY_DISABLED; /* 1: 0x0 */
  IEEE802154_TxDataFrame.fcf.framePending = 0x0; /* 1:0x0 */
  IEEE802154_TxDataFrame.fcf.ackRequired = IEEE802154_FCF_ACKNOWLEDGE_REQUIRED; /* 1: 0x1 */
#ifdef IEEE802154_ENABLE_PANID_COMPRESSION
  IEEE802154_TxDataFrame.fcf.panIdCompression = IEEE802154_FCF_PANIDCOMPRESSION_ENABLED; /* 1: 0x1 */
#else
  IEEE802154_TxDataFrame.fcf.panIdCompression = IEEE802154_FCF_PANIDCOMPRESSION_DISABLED; /* 1: 0x0 */
#endif
  IEEE802154_TxDataFrame.fcf.destinationAddressMode = CC2530BEE_Default_DestinationAdressingMode;
  IEEE802154_TxDataFrame.fcf.frameVersion = 0x00;
  IEEE802154_TxDataFrame.fcf.sourceAddressMode = CC2530BEE_Default_SourceAdressingMode;
  /* preset variable to some meaningfull values */
  IEEE802154_TxDataFrame.sequenceNumber = 0x00;
  IEEE802154_TxDataFrame.destinationPANID = CC2530BEE_Default_PanID;
  IEEE802154_TxDataFrame.destinationAddress.shortAddress = 0xffff;   /* broadcast */
  IEEE802154_TxDataFrame.sourceAddress.shortAddress = CC2530BEE_Default_ShortAddress;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[0] = IEEE_EXTENDED_ADDRESS0;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[1] = IEEE_EXTENDED_ADDRESS1;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[2] = IEEE_EXTENDED_ADDRESS2;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[3] = IEEE_EXTENDED_ADDRESS3;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[4] = IEEE_EXTENDED_ADDRESS4;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[5] = IEEE_EXTENDED_ADDRESS5;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[6] = IEEE_EXTENDED_ADDRESS6;
  IEEE802154_TxDataFrame.sourceAddress.extendedAdress[7] = IEEE_EXTENDED_ADDRESS7;
  
  config->RO_PacketizationTimeout = CC2530BEE_Default_RO_PacketizationTimeout * 10;
  
}

/**
 * Receives a frame via USART, un-escapes the data, copy the result to data
 * pointer of frame and calculates crc.
 * This function used blocking USART_getc function to read data from UART. Also 
 * make sure that enough space is be provided in frame->data pointer to receive
 * frame.
 * @param frame UART API frame with pre-filled header
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
    if ( *dataPtr == UARTFrame_Escape_Character )
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

/**
 * Adds correct header and CRC to global variable txAPIFrame. Precondition is 
 * that data in txAPIFrame.data was already filled.
 * @param data Pointer to data to be send out via USART
 * @param length number of bytes of data 
 */
void UARTAPI_sentFrame(APIFramePayload_t *data, uint16_t length)
{
  uint16_t i;
  uint8_t crc = 0;
  txAPIFrame.header.delimiter = UARTFrame_Delimiter;
  /* convert from little-endian to big-endian */
  SWAP_UINT16(length);
  txAPIFrame.header.length = length;
  USART_write((char const*)&txAPIFrame, sizeof(APIFrameHeader_t));
  for (i=0; i<length; i++)
  {
    if ( (data[i] == UARTFrame_Delimiter) || (data[i] == UARTFrame_Escape_Character) || (data[i] == UARTFrame_XON) || (data[i] == UARTFrame_XOFF) ) {
      USART_putc(UARTFrame_Escape_Character);
      USART_putc(data[i]^0x20);
    }
    else {
      USART_putc(data[i]);
    }
    crc += data[i];
  }
  crc = 0xff-crc;
  txAPIFrame.crc = crc;
  USART_putc(crc);
}

/**
 * Reads different system parametes via AT commands
 * Prepares and sents tx frame according to the atCommand requested.
 * @param data Pointer to data received within UART API frame
*/
void UARTAPI_readParameter(APIFramePayload_t *data)
{
  uint16_t atCommand;
  /* get AT command and convert to little-endian */
  atCommand = data[UARTAPI_ATCOMMAND_COMMAND] << 8 | data[UARTAPI_ATCOMMAND_COMMAND + 1];
  /* Prepare general tx frame data. Copy frame ID an AT command to sent frame */  
  txAPIFrame.data[0] = UARTAPI_ATCOMMAND_RESPONSE;
  txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_FRAMEID] = data[UARTAPI_ATCOMMAND_FRAMEID];
  txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_COMMAND] = data[UARTAPI_ATCOMMAND_COMMAND];
  txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_COMMAND + 1] = data[UARTAPI_ATCOMMAND_COMMAND + 1];
  /* as we will handle response status for each command, preset command status to "invalid command" */
  txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_STATUS] = UARTAPI_ATCOMMAND_RESPONSE_STATUS_INVALID_CMD;
  switch (atCommand)
  {
  case UARTAPI_ATCOMMAND_WRITE:
    /* not really a read but as it has no parameter it will be handled here */
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_STATUS] = UARTAPI_ATCOMMAND_RESPONSE_STATUS_OK;
    break;
  case UARTAPI_ATCOMMAND_RESTOREDEFAULTS:
    /* not really a read but as it has no parameter it will be handled here */
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_STATUS] = UARTAPI_ATCOMMAND_RESPONSE_STATUS_OK;
    break;
  case UARTAPI_ATCOMMAND_SOFTWARERESET:
    /* not really a read but as it has no parameter it will be handled here */
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_STATUS] = UARTAPI_ATCOMMAND_RESPONSE_STATUS_OK;
    /* Trigger watchdog one more time to make sure that UART tx buffer is empty and then for the watchdog to reset */
    UARTAPI_sentFrame(txAPIFrame.data, UARTAPI_ATCOMMAND_RESPONSE_DATA);
    WDT_trigger();
    while (1)
    {
      nop();
    }
    break;
  case UARTAPI_ATCOMMAND_CHANNEL:
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_STATUS] = UARTAPI_ATCOMMAND_RESPONSE_STATUS_OK;
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA] = CC2530Bee_Config.IEEE802154_config.Channel;
    UARTAPI_sentFrame(txAPIFrame.data, UARTAPI_ATCOMMAND_RESPONSE_DATA + sizeof(uint8_t));
    break;
  case UARTAPI_ATCOMMAND_PANID:
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_STATUS] = UARTAPI_ATCOMMAND_RESPONSE_STATUS_OK;
    *((IEEE802154_PANIdentifier_t*)&(txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA])) = CC2530Bee_Config.IEEE802154_config.PanID;
    UARTAPI_sentFrame(txAPIFrame.data, UARTAPI_ATCOMMAND_RESPONSE_DATA + sizeof(IEEE802154_PANIdentifier_t));
    break;
  case UARTAPI_ATCOMMAND_DESTINATIONADDRESSHIGH:
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_STATUS] = UARTAPI_ATCOMMAND_RESPONSE_STATUS_OK;
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 0] = IEEE802154_TxDataFrame.destinationAddress.extendedAdress[4];
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 1] = IEEE802154_TxDataFrame.destinationAddress.extendedAdress[5];
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 2] = IEEE802154_TxDataFrame.destinationAddress.extendedAdress[6];
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 3] = IEEE802154_TxDataFrame.destinationAddress.extendedAdress[7];
    UARTAPI_sentFrame(txAPIFrame.data, UARTAPI_ATCOMMAND_RESPONSE_DATA + sizeof(IEEE802154_ExtendedAddress_t)/2);
    break;
  case UARTAPI_ATCOMMAND_DESTINATIONADDRESSLOW:
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_STATUS] = UARTAPI_ATCOMMAND_RESPONSE_STATUS_OK;
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 0] = IEEE802154_TxDataFrame.destinationAddress.extendedAdress[0];
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 1] = IEEE802154_TxDataFrame.destinationAddress.extendedAdress[1];
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 2] = IEEE802154_TxDataFrame.destinationAddress.extendedAdress[2];
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 3] = IEEE802154_TxDataFrame.destinationAddress.extendedAdress[3];
    UARTAPI_sentFrame(txAPIFrame.data, UARTAPI_ATCOMMAND_RESPONSE_DATA + sizeof(IEEE802154_ExtendedAddress_t)/2);
    break;
  case UARTAPI_ATCOMMAND_SOURCEADDRESS16BIT:
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_STATUS] = UARTAPI_ATCOMMAND_RESPONSE_STATUS_OK;
    *((IEEE802154_ShortAddress_t*)&(txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA])) = IEEE802154_TxDataFrame.sourceAddress.shortAddress;
    UARTAPI_sentFrame(txAPIFrame.data, UARTAPI_ATCOMMAND_RESPONSE_DATA + sizeof(IEEE802154_ShortAddress_t));
    break;
  case UARTAPI_ATCOMMAND_SERIALNUMBERHIGH:
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_STATUS] = UARTAPI_ATCOMMAND_RESPONSE_STATUS_OK;
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 0] = IEEE802154_TxDataFrame.sourceAddress.extendedAdress[4];
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 1] = IEEE802154_TxDataFrame.sourceAddress.extendedAdress[5];
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 2] = IEEE802154_TxDataFrame.sourceAddress.extendedAdress[6];
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 3] = IEEE802154_TxDataFrame.sourceAddress.extendedAdress[7];
    UARTAPI_sentFrame(txAPIFrame.data, UARTAPI_ATCOMMAND_RESPONSE_DATA + sizeof(IEEE802154_ExtendedAddress_t)/2);
    break;
  case UARTAPI_ATCOMMAND_SERIALNUMBERLOW:
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_STATUS] = UARTAPI_ATCOMMAND_RESPONSE_STATUS_OK;
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 0] = IEEE802154_TxDataFrame.sourceAddress.extendedAdress[0];
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 1] = IEEE802154_TxDataFrame.sourceAddress.extendedAdress[1];
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 2] = IEEE802154_TxDataFrame.sourceAddress.extendedAdress[2];
    txAPIFrame.data[UARTAPI_ATCOMMAND_RESPONSE_DATA + 3] = IEEE802154_TxDataFrame.sourceAddress.extendedAdress[3];
    UARTAPI_sentFrame(txAPIFrame.data, UARTAPI_ATCOMMAND_RESPONSE_DATA + sizeof(IEEE802154_ExtendedAddress_t)/2);
    break;
  default:
    break;
  }
}

void UARTAPI_setParameter(APIFramePayload_t *dta)
{
}

/**
 * Callback whenever Beacon frame was received
 * @param payloadLength Length of data in IEEE802154_RxDataFrame.payload
 * @param rssi value measured over the firs eight symbols following SFD
 * @note This function runs in interrupt context
*/
void IEEE802154_UserCbk_BeaconFrameReceived(uint8_t payloadLength, sint8_t rssi)
{
  
}

/**
 * Callback whenever Data frame was received
 * @param payloadLength Length of data in IEEE802154_RxDataFrame.payload
 * @param rssi value measured over the firs eight symbols following SFD
 * @note This function runs in interrupt context
*/
void IEEE802154_UserCbk_DataFrameReceived(uint8_t payloadLength, sint8_t rssi)
{
  uint8_t *payloadDataPtr = txAPIFrame.data;
  if (IEEE802154_RxDataFrame.fcf.sourceAddressMode == IEEE802154_FCF_ADDRESS_MODE_64BIT)
  {
    *(payloadDataPtr++) = UARTAPI_RECEIVE_PACKAGE_64BIT;
    txAPIFrame.header.length = payloadLength + UARTAPI_64BITRECEIVE_HEADER_SIZE;
    memcpy(payloadDataPtr, &(IEEE802154_RxDataFrame.sourceAddress.extendedAdress), sizeof(IEEE802154_ExtendedAddress_t) );
    payloadDataPtr += sizeof(IEEE802154_ExtendedAddress_t);
  }
  else if (IEEE802154_RxDataFrame.fcf.sourceAddressMode == IEEE802154_FCF_ADDRESS_MODE_16BIT)
  {
    *(payloadDataPtr++) = UARTAPI_RECEIVE_PACKAGE_16BIT;
    txAPIFrame.header.length = payloadLength + UARTAPI_16BITRECEIVE_HEADER_SIZE;
    *(payloadDataPtr++) = HI_UINT16(IEEE802154_RxDataFrame.sourceAddress.shortAddress);
    *(payloadDataPtr++) = LO_UINT16(IEEE802154_RxDataFrame.sourceAddress.shortAddress);
  }
  else /* IEEE802154_FCF_ADDRESS_MODE_NONE */
  {
    *(payloadDataPtr++) = UARTAPI_RECEIVE_PACKAGE_NONE;
    txAPIFrame.header.length = payloadLength + UARTAPI_NONERECEIVE_HEADER_SIZE;
  }
  *(payloadDataPtr++) = rssi;
  uint8_t optionByte = 0x00;
  if (IEEE802154_RxDataFrame.destinationAddress.shortAddress == IEEE802154_BROADCAST_ADDRESS_16BIT)
  {
    optionByte |= UARTAPI_RECEVICE_OPTIONS_ADDRESS_BROADCAST;
  }
  if (IEEE802154_RxDataFrame.destinationPANID == IEEE802154_BROADCAST_PAN_ID)
  {
    optionByte |= UARTAPI_RECEVICE_OPTIONS_PAN_BROADCAST;
  }
  *(payloadDataPtr++) = optionByte;
  memcpy(payloadDataPtr, IEEE802154_RxDataFrame.payload, payloadLength );
  UARTAPI_sentFrame(txAPIFrame.data, txAPIFrame.header.length);
}

/**
 * Callback whenever Ack frame was received
 * @param payloadLength Length of data in IEEE802154_RxDataFrame.payload
 * @param rssi value measured over the firs eight symbols following SFD
 * @note This function runs in interrupt context
*/
void IEEE802154_UserCbk_AckFrameReceived(uint8_t payloadLength, sint8_t rssi)
{
  txAPIFrame.header.length = UARTAPI_TX_STATUS_HEADER_SIZE + UARTAPI_TX_STATUS_PAYLOAD_SIZE;
  txAPIFrame.data[0] = UARTAPI_TRANSMIT_STATUS;
  txAPIFrame.data[UARTAPI_TX_STATUS_FRAME_ID] = IEEE802154_RxDataFrame.sequenceNumber;
  txAPIFrame.data[UARTAPI_TX_STATUS_STATUS_BYTE] = UARTAPI_TX_STATUS_SUCCESS;
  UARTAPI_sentFrame(txAPIFrame.data, txAPIFrame.header.length);
}

/**
 * Callback whenever MAC Command frame was received
 * @param payloadLength Length of data in IEEE802154_RxDataFrame.payload
 * @param rssi value measured over the firs eight symbols following SFD
 * @note This function runs in interrupt context
*/
void IEEE802154_UserCbk_MACCommandFrameReceived(uint8_t payloadLength, sint8_t rssi)
{
  
}

/**
 * Callback whenever frame with incorrect CRC was received
 * @param payloadLength Length of data in IEEE802154_RxDataFrame.payload
 * @param rssi value measured over the firs eight symbols following SFD
 * @note This function runs in interrupt context
*/
void IEEE802154_UserCbk_CRCError(uint8_t payloadLength, sint8_t rssi)
{
  
}

/** @}*/