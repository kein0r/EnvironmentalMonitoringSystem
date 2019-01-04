#include <Arduino.h>
#include "AMREnergyMonitor.h"
#include <ArduinoOTA.h>
#include <Homie.h>
#include <SPI.h>
#include <RH_RF69.h>
#include <ESP8266mDNS.h>

/*
 * Information regarding automatic meter reading is mostly taken from the following transmitTimeOffset
 * - https://www.mathworks.com/help/supportpkg/plutoradio/examples/automatic-meter-reading.html
 * - https://github.com/bemasher/rtlamr
 * RF hardware https://learn.adafruit.com/adafruit-rfm69hcw-and-rfm96-rfm95-rfm98-lora-packet-padio-breakouts
 * Library for that module http://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF69.html
- Baudrate
Modulation
From http://www.smartmetereducationnetwork.com/uploads/how-to-tell-if-I-have-a-ami-dte-smart-advanced-meter/Itron%20Centron%20Meter%20Technical%20Guide1482163-201106090057150.pdf

Utilizing the 96-bit Itron® Standard Consumption Message protocol (SCM), the C1SR
provides the energy (kWh) consumption, module ID number, tamper indications, meter
type, and error checking information in each radio frequency transmission. Within the 96
-bit SCM, 26 bits are allocated to the module ID number for meter identification (also
referred to as ERT ID number). As of October 15, 1999, all CENTRON C1SR meters utilize 26
-bit identification numbers.
The C1SR uses frequency hopping and transmits within the unlicensed 910 to 920 MHz
band on an average of once per second. In order to avoid interference from other devices,
the transmission frequencies and time interval between transmission cycles are completely
random in nature.

The RF transmission is spread spectrum using Binary Phase Shift Keyed (BPSK) modulation. A chipping signal
is used to spread the carrier and actual data is on-off-keyed (OOK). The modulator output drives the
amplifier chain that provides a signal of approximately 100mW. The antenna is tuned for maximum efficiency
at the carrier frequency

Interval Data Message (IDM Message)
 * Training sync        0x5555  (2 byte, fix)
 * Frame sync           0x16a3  (2 byte, fix)
 * Packet type          0x1c    (1 byte, fix)
 * Packet length        0x5cc6  (2 byte, fix)
 * Application version  0x04    (1 byte, fix)
 * ERT type             0x17    (1 byte, last 4 bits determine type)
 * Preample 20 bits     matches: 0b111110010101001100000 = 0x1F2A60 from https://github.com/bemasher/rtlamr

*/

RH_RF69::ModemConfigChoice modemConfig = RH_RF69::OOK_Rb1Bw1;
uint8_t IDMMessageSyncWord[] = { 0x16, 0xa3 };

unsigned long nextStatusTime = 0;
unsigned long nextModemConfigChange = 0;
uint8_t statusLEDState = LOW;

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_IRQ);

void setupHandler() {
  pinMode(RFM69_RST, OUTPUT);
  pinMode(STATUSLED, OUTPUT);

  digitalWrite(RFM69_RST, LOW);

  // reset RF module
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (rf69.init())
  {
    Serial.println("RFM69 successfulyl initialized");
  }
  rf69.setModemConfig(RH_RF69::OOK_Rb32Bw64);
  rf69.setPreambleLength(IDM_PREAMBLELENGTH);
  rf69.setSyncWords(IDMMessageSyncWord, IDM_SYNCWORDLENGTH);
  if (rf69.setFrequency(RF69_FREQ)) {
    /* If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
     * ishighpowermodule flag set like this: */
    rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
    Serial.println("RFM69 frequency set");
  }

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void loopHandler()
{
  if (millis() > nextStatusTime)
  {
    digitalWrite(STATUSLED, statusLEDState);
    statusLEDState = (statusLEDState == HIGH) ? LOW : HIGH;
    nextStatusTime = millis() + 1000;
  }
/*
  if (millis() > nextModemConfigChange)
  {
    if (modemConfig != RH_RF69::OOK_Rb32Bw64)
    {
      modemConfig += 1;
    }
    else
    {
      modemConfig = RH_RF69::OOK_Rb1Bw1;
    }
    Serial.print("Set ModemConfig to: ");
    Serial.println(modemConfig);
    rf69.setModemConfig(modemConfig);
  }*/
  if (rf69.available())
  {
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
  ArduinoOTA.handle();
}

void setup()
{
  Serial.begin(115200);
  /* Connect to WiFi, wait until connection was established */
  Homie_setFirmware("AMREnergyMonitor", "0.0.1");
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
  Homie.setup();
}

void loop()
{
  Homie.loop();
}
