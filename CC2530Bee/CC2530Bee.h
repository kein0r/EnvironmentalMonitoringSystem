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
#define IEEE802154_Default_Channel                      25

/**
 * Default Short address of this note
*/
#define IEEE802154_Default_ShortAddress                 (uint16_t)0xaffe

/**
 * Default PanID of this note
*/
#define IEEE802154_Default_PanID                        (uint16_t)0x3332

/**
 * Default source and destination address mode
*/
#define IEEE802154_Default_DestinationAdressingMode     IEEE802154_FCF_ADDRESS_MODE_16BIT
#define IEEE802154_Default_SourceAdressingMode          IEEE802154_FCF_ADDRESS_MODE_16BIT


/**
 * Default RO Time in multiple of character times
*/
#define IEEE802154_Default_RO_PacketizationTimeout      (uint8_t)0x03

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
} CC2530Bee_Cmd;

/*******************| Global variables |*******************************/

/*******************| Function prototypes |****************************/

void CC2530Bee_loadConfig(CC2530Bee_Config_t *config);

#endif
/** @}*/