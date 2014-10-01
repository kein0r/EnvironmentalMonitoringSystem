/** @ingroup CC2530Bee
 * @{
 */
#ifndef CC2530BEE_H_
#define CC2530BEE_H_
   
/*******************| Inclusions |*************************************/
#include <PlatformTypes.h>
#include <USART.h>
   
/*******************| Macros |*****************************************/

/*******************| Type definitions |*******************************/
/**
 * \brief Struct to store all kind of configuration data for CC2530Bee.
 * This variable will be stored in EEPROM
 */
typedef struct {
  USART_Baudrate_t USART_Baudrate;
  USART_Parity_t USART_Parity;
  uint16_t crc;                 /*!< crc to be saved in EEPROM to check if data is valid */
} CC2530Bee_Config_t;


/*******************| Global variables |*******************************/

/*******************| Function prototypes |****************************/

void CC2530Bee_loadConfig(CC2530Bee_Config_t *config);

#endif
/** @}*/