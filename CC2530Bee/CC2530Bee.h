/** @ingroup CC2530Bee
 * @{
 */
#ifndef CC2530BEE_H_
#define CC2530BEE_H_
   
/*******************| Inclusions |*************************************/
#include <PlatformTypes.h>
#include <USART.h>
#include <IEEE_802.15.4.h>
   
/*******************| Macros |*****************************************/
   
/** 
 * Channel to be used for IEEE 802.15.4 radio. The channels are numbered 11 
 * through 26.
*/
#define CC2530BEE_Default_Channel                       25

/**
 * Default Short address of this note
*/
#define CC2530BEE_Default_ShortAddress                  (IEEE802154_ShortAddress_t)0xaffe

/**
 * Value for short address to note that 64bit addressing is to be used.
*/
#define CC2530BEE_USE_64BIT_ADDRESSING                  (IEEE802154_ShortAddress_t)0xfffe

/**
 * Default PanID of this note
*/
#define CC2530BEE_Default_PanID                         (IEEE802154_PANIdentifier_t)0x3332

/**
 * Default source and destination address mode
*/
#define CC2530BEE_Default_DestinationAdressingMode      IEEE802154_FCF_ADDRESS_MODE_16BIT
#define CC2530BEE_Default_SourceAdressingMode           IEEE802154_FCF_ADDRESS_MODE_16BIT


/**
 * Default RO Time in multiple of character times
*/
#define CC2530BEE_Default_RO_PacketizationTimeout       (uint8_t)0x03
   
/** 
 * UART crc ok
*/
#define UARTFrame_CRC_OK                                (uint8_t)0x01
   
/** 
 * UART crc not ok
*/
#define UARTFrame_CRC_Not_OK                            (uint8_t)0x00

/**
 * UART rx frame escape mask
*/
#define UARTFrame_Escape_Mask                           (uint8_t)0x20

/**
 * UART rx frame delimiter (needs to be escaped)
*/
#define UARTFrame_Delimiter                             (uint8_t)0x7e

/**
 * UART rx frame escape character (needs to be escaped)
*/
#define UARTFrame_Escape_Character                      (uint8_t)0x7d

/**
 * UART rx frame XON character (needs to be escaped)
*/
#define UARTFrame_XON                                   (uint8_t)0x11

/**
 * UART rx frame XOFF character (needs to be escaped)
*/
#define UARTFrame_XOFF                                  (uint8_t)0x13

#define UARTAPI_MODEMSTATUS                             (uint8_t)0x8a
#define UARTAPI_ATCOMMAND                               (uint8_t)0x08
#define UARTAPI_ATCOMMAND_QUEUE                         (uint8_t)0x09
#define UARTAPI_ATCOMMAND_RESPONSE                      (uint8_t)0x88
#define UARTAPI_REMOTE_AT_COMMAND_REQUEST               (uint8_t)0x17
#define UARTAPI_REMOTE_AT_COMMAND_RESPONSE              (uint8_t)0x97
#define UARTAPI_TRAMSMIT_REQUEST_64BIT                  (uint8_t)0x00
#define UARTAPI_TRAMSMIT_REQUEST_16BIT                  (uint8_t)0x01
#define UARTAPI_TRANSMIT_STATUS                         (unit8_t)0x89
#define UARTAPI_RECEIVE_PACKAGE_64BIT                   (uint8_t)0x80
#define UARTAPI_RECEIVE_PACKAGE_16BIT                   (uint8_t)0x81


#define UARTAPI_MODEMSTATUS_HARDWARE_RESET              (uint8_t)0x00
#define UARTAPI_MODEMSTATUS_WATCHDOG_RESET              (uint8_t)0x00
#define UARTAPI_MODEMSTATUS_ASSOCIATED                  (uint8_t)0x00
#define UARTAPI_MODEMSTATUS_DISASSOCIATED               (uint8_t)0x00
#define UARTAPI_MODEMSTATUS_SYNC_LOST                   (uint8_t)0x00
#define UARTAPI_MODEMSTATUS_COORDINATOR_REALIGNMENT     (uint8_t)0x00
#define UARTAPI_MODEMSTATUS_COORDINATOR_STARTED         (uint8_t)0x00

#define UARTAPI_TRANSMIT_OPTIONS_DISABLEACK             (uint8_t)0x01
#define UARTAPI_TRANSMIT_OPTIONS_BROADCASTPANID         (uint8_t)0x04

#define UARTAPI_16BITTRANSMIT_FRAMEID                   (uint8_t)0x01
#define UARTAPI_16BITTRANSMIT_ADDRESS                   (uint8_t)0x02
#define UARTAPI_16BITTRANSMIT_OPTIONS                   (uint8_t)0x04
#define UARTAPI_16BITTRANSMIT_DATA                      (uint8_t)0x05

#define UARTAPI_64BITTRANSMIT_FRAMEID                   (uint8_t)0x01
#define UARTAPI_64BITTRANSMIT_ADDRESS                   (uint8_t)0x02
#define UARTAPI_64BITTRANSMIT_OPTIONS                   (uint8_t)0x0a
#define UARTAPI_64BITTRANSMIT_DATA                      (uint8_t)0x0b

/*******************| Type definitions |*******************************/

/**
 * \brief Struct to store all kind of configuration data for CC2530Bee.
 * This variable will be stored in EEPROM
 */
typedef struct {
  USART_Baudrate_t USART_Baudrate;
  USART_Parity_t USART_Parity;
  IEEE802154_Config_t IEEE802154_config;
  IEEE802154_DataFrameHeader_t  IEEE802154_TxDataFrame;
  IEEE802154_DataFrameHeader_t  IEEE802154_RxDataFrame;
  uint8_t RO_PacketizationTimeout;  /*!< Timout in milliseconds after which data received via UART will be packed and sent via radio. */
  uint8_t crc;                 /*!< crc to be saved in EEPROM to check if data is valid */
} CC2530Bee_Config_t;

typedef struct {
  uint8_t startDelimiter;
  uint16_t length;
} CC2530CmdHeader_t;

typedef uint8_t CC2530Bee_CmdFrameData;
typedef struct {
  CC2530CmdHeader_t header;
  CC2530Bee_CmdFrameData *cmdFrameDataPtr;
  uint8_t crc;
} CC2530Bee_Cmd_t;

/**
 * Struct to send/receive API frame header.
 * @note: Only works on 8-bit systems or if #pragma pack is used
*/
typedef struct {
  uint8_t delimiter; 
  uint16_t length;
} APIFrameHeader_t;

typedef uint8_t APIFramePayload_t;

typedef struct {
  APIFrameHeader_t header;
  APIFramePayload_t *data;
  uint8_t crc;
} APIFrame_t;

/*******************| Global variables |*******************************/

/*******************| Function prototypes |****************************/

void CC2530Bee_loadConfig(CC2530Bee_Config_t *config);

uint8_t UARTAPI_receiveFrame(APIFrame_t *frame);

#endif
/** @}*/