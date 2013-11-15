/** @ingroup DHT22
 * @{
 */
#ifndef IEEE_802_15_4_H_CFG_H_
#define IEEE_802_15_4_H_CFG_H_

/*******************| Inclusions |*************************************/
#include <PlatformTypes.h>
   
/*******************| Macros |*****************************************/ 
/**
* Add explenation here. TODO: Rename
*/
#define RADIO_ACK_PACKET_SIZE                   5

/**
* Add explenation here
*/
#define IEE802154_ENABLE_PANID_COMPRESSION      STD_ON

/** 
 * Channel to be used for IEEE 802.15.4 radio. The channels are numbered 11 
 * through 26.
*/
#define IEEE802154_Channel                      25

/**
 * Short address of this note
*/
#define IEEE802154_ShortAddress                 (uint16)0x0033

/**
 * Short address of this note
*/
#define IEEE802154_PanID                        (uint16)0xaffe
   
/*******************| Type definitions |*******************************/

/*******************| Global variables |*******************************/

/*******************| Function prototypes |****************************/

#endif
/** @}*/