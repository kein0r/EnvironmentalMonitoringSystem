/** @ingroup IEEE_802.15.4
 * @{
 */
#ifndef IEEE_802_15_4_H_CFG_H_
#define IEEE_802_15_4_H_CFG_H_

/*******************| Inclusions |*************************************/
#include <PlatformTypes.h>
   
/*******************| Macros |*****************************************/ 
/**
 * Sets whether or not PanID should be available twice in frame frame header (not defined)
 * or could be ommited (defined) and assumed that source PanID equals to destination
 * PanID.
 */
#define IEEE802154_ENABLE_PANID_COMPRESSION
     
/*******************| Type definitions |*******************************/

/*******************| Global variables |*******************************/

/*******************| Function prototypes |****************************/

#endif
/** @}*/