/** @ingroup CC2530Bee
 * @{
 */
#ifndef CC2530BEE_H_
#define CC2530BEE_H_
   
/*******************| Inclusions |*************************************/
#include <PlatformTypes.h>
   
/*******************| Macros |*****************************************/

/*******************| Type definitions |*******************************/
typedef enum {
  CC2530Bee_SleepMode_None;
} CC2530Bee_SleepMode_t;

/**
 * \brief Struct to store all kind of configuration data for CC2530Bee.
 * This variable will be stored in EEPROM
 */
typedef struct {
  USART_Baudtate_t USART_Baudrate;
  CC2530Bee_SleepMode_t sleepMode;
  uint16_t crc;                 /*!< crc to be saved in EEPROM to check if data is valid */
} CC2530Bee_Config_t;


/*******************| Global variables |*******************************/

/*******************| Function prototypes |****************************/

#endif
/** @}*/