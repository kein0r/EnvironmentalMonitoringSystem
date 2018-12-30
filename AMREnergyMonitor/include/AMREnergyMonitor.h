#ifndef AMRENERGYMONITOR_H
#define AMRENERGYMONITOR_H

/**
 * Center frequency in MHz. 240.0 to 960.0
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
#define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ)
#define RFM69_RST     2    // "D"
#define STATUSLED     0

#endif
