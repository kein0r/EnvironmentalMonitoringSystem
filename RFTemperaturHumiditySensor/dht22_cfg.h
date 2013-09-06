/** @ingroup DHT22
 * @{
 */

#ifndef DHT22_CFG_H_
#define DHT22_CFG_H_
   
/*
 * Cycle time of the DHT22 main function in ms
*/   
#define DHT22_MAINFUNCTION_CYCLETIME            (uint8)1

/*
 * Platform specific function to set data line to output
*/ 
#define DHT22_SetDataLineOutput()

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

#endif
/** @}*/
