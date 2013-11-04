/** @ingroup DHT22
 * @{
 */
#ifndef DHT22_CFG_H_
#define DHT22_CFG_H_

/*******************| Inclusions |*************************************/
#include <board.h>
#include <ioCC2530.h>
   
/*******************| Macros |*****************************************/   

#define DHT22_DATALINE_LOW              (uint8)HAL_LOW
#define DHT22_DATALINE_HIGH             (uint8)HAL_HIGH

/*
 * Platform specific function to set data line to output
*/ 
#define DHT22_SetDataLineOutput()       P0DIR_2 = HAL_PINOUTPUT

/*
 * Platform specific function to set data line to input
*/ 
#define DHT22_SetDataLineInput()        P0DIR_2 = HAL_PININPUT

/*
 * Platform specific function to write logic low to sensor
*/ 
#define DHT22_WriteDataBitLow()         P0_2 = DHT22_DATALINE_LOW

/*
 * Platform specific function to write logic high to sensor
*/ 
#define DHT22_WriteDataBitHigh()        P0_2 = DHT22_DATALINE_HIGH

/*
 * Platform specific function to read bit from data line
*/ 
#define DHT22_ReadDataBit()             P0_2

/* 
 **************************************************************************
 * Usually nothing has to be changed below this line. Change values only if
 * your sensor or set-up needs different timing than normal.
 */

/*
 * Time for the start signal of the MCU in uS. According to datasheet at least 
 * 500ms better between 1ms and 10ms
*/
#define DHT22_MCUSendStartSignalTime    1000

/*
 * Time to wait for sensor response after start signal in uS. According to 
 * datasheet 20-40uS
*/
#define DHT22_MCUWaitForSensorResponse  40
   
/*******************| Type definitions |*******************************/

/*******************| Global variables |*******************************/

/*******************| Function prototypes |****************************/

#endif
/** @}*/
