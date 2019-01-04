#ifndef AMRENERGYMONITOR_H
#define AMRENERGYMONITOR_H

#include <Arduino.h>

/**
 * Center frequency in MHz. 900.0 to 960.0
 * According to https://github.com/bemasher/rtlamr/blob/master/scm/scm.go 912MHz
 * According to https://www.mathworks.com/help/supportpkg/plutoradio/examples/automatic-meter-reading-1.html 915MHz
 */
#define RF69_FREQ         (float)915.0

/**
 * Define TX power. Range from 14-20
 */
#define RF69_TXPOWER      (int8_t)20

/**
 * Define pins used to connect to RF module
 */
#define RFM69_CS      15   // "E"
#define RFM69_IRQ     4    // G0 on rf board
#define RFM69_RST     2    // "D"
#define STATUSLED     0


/*
 * Interval Data Message (IDM) related stuff
*/

extern uint8_t IDMMessageSyncWord[];

#define IDM_PREAMBLELENGTH     (uint8_t)2
#define IDM_SYNCWORDLENGTH     (uint8_t)sizeof(IDMMessageSyncWord)

typedef struct {
  uint8_t packetType;               /* 0x1c according to information from internet */
  uint16_t packetLength;            /* Firt byte is number of bytes (0x5c), last byte is hemming code of the first byte (0xc6) */
  uint8_t applicationVersion;       /* Should be 0x04 */
  uint8_t ERTType;                  /* Last 4 bits determine the type of ERT */
  uint32_t ERTSerialNumber;
  uint8_t consumptionIntervalCount;
  uint8_t moduleProgrammingState;
  uint8_t tamperCount[6];
  uint16_t asyncCount;
  uint8_t powerOutageFlags[6];
  uint32_t lastConsumptionCount;
  uint8_t differentialConsumptionIntervals[53]; /* 47 9-bit integer (metered consumption), leftmost interval is most recent */
  uint16_t transmitTimeOffset;      /* Time elapsed since last interval ended */
  uint16_t serialNumberCRC;         /* CRC-16-CCITT of ERT serial no */
  uint16_t messageCRT;              /* CRC-16-CCITT of complete messae */
} IntervalDataMessage_t;

#endif
