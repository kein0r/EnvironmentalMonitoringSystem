/** @ingroup DHT22
 * @{
 */
#ifndef DHT22_CFG_H_
#define DHT22_CFG_H_

/*******************| Inclusions |*************************************/
#include <board.h>
   
/*******************| Macros |*****************************************/   
/*
 * Platform specific function to set data line to output
*/ 
#define DHT22_SetDataLineOutput() P0DIR = 

/*
 * Platform specific function to set data line to input
*/ 
#define DHT22_SetDataLineInput()

/*
 * Platform specific function to write logic low to sensor
*/ 
#define DHT22_WriteDataBitLow()                   

/*
 * Platform specific function to write logic high to sensor
*/ 
#define DHT22_WriteDataBitHigh()

/*
 * Platform specific function to read bit from data line
*/ 
#define DHT22_ReadDataBit()             DHT22_DATALINE_LOW

#define DHT22_DATALINE_LOW              (uint8)0
#define DHT22_DATALINE_HIGH             (uint8)1

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
