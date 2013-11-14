/** @ingroup Platform
 * @{
 */
#ifndef PLATFORMTYPES_H_
#define PLATFORMTYPES_H_

/*******************| Inclusions |*************************************/
   
/*******************| Macros |*****************************************/
#define STD_ON                  1
#define STD_OFF                 0

/**
 * Macros to access high and low part of 16bit word separately 
*/
#define HI_UINT16(a)            (((uint16)(a) >> 8) & 0xFF)
#define LO_UINT16(a)            ((uint16)(a) & 0xFF)
   
/*******************| Type definitions |*******************************/

/* Data lenght according to EW8051_CompilerReference.pdf page 276 */
typedef unsigned char uint8;
typedef signed char sint8;

typedef unsigned short uint16;
typedef signed short sint16;

typedef unsigned long uint32;
typedef signed long sint32;

/*******************| Global variables |*******************************/

/*******************| Function prototypes |****************************/

void delay_us(uint16 usec);
#endif

/** @}*/
