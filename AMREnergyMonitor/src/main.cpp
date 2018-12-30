#include <Arduino.h>
#include <RH_RF69.h>
#include "AMREnergyMonitor.h"

/*
 * RF hardware https://learn.adafruit.com/adafruit-rfm69hcw-and-rfm96-rfm95-rfm98-lora-packet-padio-breakouts
 * Library for that module http://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF69.html
- Baudrate
Modulation
- (G)FSK or (G)MSK? ASK OOK?
From http://www.smartmetereducationnetwork.com/uploads/how-to-tell-if-I-have-a-ami-dte-smart-advanced-meter/Itron%20Centron%20Meter%20Technical%20Guide1482163-201106090057150.pdf
The RF transmission is spread spectrum using Binary Phase Shift Keyed (BPSK) modulation. A chipping signal
is used to spread the carrier and actual data is on-off-keyed (OOK). The modulator output drives the
amplifier chain that provides a signal of approximately 100mW. The antenna is tuned for maximum efficiency
at the carrier frequency
*/

typedef enum {ENERGYMONITORSTATE_NONE,
              ENERGYMONITORSTATE_INITIALIZED,
              ENERGYMONITORSTATE_FREQUENCYSET
} EnergyMonitorState_t;
EnergyMonitorState_t EnergyMonitorState = ENERGYMONITORSTATE_NONE;

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_IRQ);

void setup() {
  Serial.begin(115200);
  pinMode(RFM69_RST, OUTPUT);

  digitalWrite(RFM69_RST, LOW);

  // reset RF module
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (rf69.init())
  {
    Serial.println("RFM69 successfulyl initialized");
    EnergyMonitorState = ENERGYMONITORSTATE_INITIALIZED;
  }
  if (rf69.setFrequency(RF69_FREQ)) {
    /* If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
     * ishighpowermodule flag set like this: */
    rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
    EnergyMonitorState = ENERGYMONITORSTATE_FREQUENCYSET;
    Serial.println("RFM69 frequency set");
  }
}

void loop()
{
  if (EnergyMonitorState == ENERGYMONITORSTATE_FREQUENCYSET) {
    if (rf69.available()) {
      // Should be a message for us now
      uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
      if (rf69.recv(buf, &len)) {
        if (!len) return;
        buf[len] = 0;
        Serial.print("Received [");
        Serial.print(len);
        Serial.print("]: ");
        Serial.println((char*)buf);
        Serial.print("RSSI: ");
        Serial.println(rf69.lastRssi(), DEC);

        if (strstr((char *)buf, "Hello World")) {
          // Send a reply!
          uint8_t data[] = "And hello back to you";
          rf69.send(data, sizeof(data));
          rf69.waitPacketSent();
          Serial.println("Sent a reply");
        }
      } else {
        Serial.println("Receive failed");
      }
    }
  }
}
