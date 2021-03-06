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
*/

uint8_t IDMMessageSyncWord[] = { 0x16, 0xa3 };

uint8_t rxBufferIDM[0x5c];
bool rxBufferValid = false;

unsigned long nextStatusTime = 0;
unsigned long nextModemConfigChange = 0;
uint8_t statusLEDState = LOW;

sint8_t lastRSSI = 0;
uint8_t lastCRCOk = 0;

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_IRQ);

void serialPrintBinary(uint16_t content)
{
  for (int i=0; i<8; i++)
  {
    if ((i%4) == 0) Serial.print(" ");
    content & 0x80 ? Serial.print('1') : Serial.print('0');
    content = content << 1;
  }
}

void myRF69ISR()
{
  // Get the interrupt cause
  ATOMIC_BLOCK_START;
  uint8_t irqflags2 = rf69.spiRead(RH_RF69_REG_28_IRQFLAGS2);
  if (irqflags2 & RH_RF69_IRQFLAGS2_PAYLOADREADY)
  {
	  // A complete message has been received with (good) CRC
	  lastRSSI = -((int8_t)(rf69.spiRead(RH_RF69_REG_24_RSSIVALUE) >> 1));
    lastCRCOk = rf69.spiRead((RH_RF69_REG_28_IRQFLAGS2 & RH_RF69_IRQFLAGS2_CRCOK) >> 1);

	  rf69.setModeIdle();
    rf69.spiBurstRead(RH_RF69_REG_00_FIFO, rxBufferIDM, 0x5c-2-2);
    rxBufferValid = true;
  }
  ATOMIC_BLOCK_END;
}

bool myAvailable()
{
  rf69.setModeRx(); // Make sure we are receiving
  return rxBufferValid;
}

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
  if (rf69.setFrequency(RF69_FREQ)) {
    /* If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
     * ishighpowermodule flag set like this: */
    rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
    Serial.println("RFM69 frequency set");
  }
  rf69.setModemConfig(RH_RF69::OOK_Rb32Bw64);
  //rf69.spiWrite(RH_RF69_REG_03_BITRATEMSB, 1);
  //rf69.spiWrite(RH_RF69_REG_04_BITRATELSB, 0xf4);
  /* Change to fix-length, Manchester encoding and set to trigger ISR even if CRC does not match. */
  rf69.spiWrite(RH_RF69_REG_37_PACKETCONFIG1, (RH_RF69_PACKETCONFIG1_PACKETFORMAT_VARIABLE | RH_RF69_PACKETCONFIG1_DCFREE_MANCHESTER  | RH_RF69_PACKETCONFIG1_CRCAUTOCLEAROFF | RH_RF69_PACKETCONFIG1_ADDRESSFILTERING_NONE));
  rf69.spiWrite(RH_RF69_REG_38_PAYLOADLENGTH, 0x5c-2-2);  /* 0x5c minus preamble and CRC (each two bytes) */
  rf69.setPreambleLength(IDM_PREAMBLELENGTH);
  /* map interrupt to own function */
  attachInterrupt(digitalPinToInterrupt(RFM69_IRQ), myRF69ISR, RISING);
  /* rf69.setSyncWords(IDMMessageSyncWord, IDM_SYNCWORDLENGTH); */
  rf69.setSyncWords(IDMMessageSyncWord, 0);
#if 0
  /* go back to continous mode for now */
  rf69.spiWrite(RH_RF69_REG_2E_SYNCCONFIG, 0b00000000); /* SyncOn = 0 */
  rf69.spiWrite(RH_RF69_REG_02_DATAMODUL, (0b01000000 | RH_RF69_DATAMODUL_MODULATIONTYPE_OOK | RH_RF69_DATAMODUL_MODULATIONSHAPING_OOK_NONE));
#endif
  Serial.println("RFM69 initialization complete!");

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
  if (Serial.available())
  {
    /* Empty inc stream */
    while (Serial.available() > 0)
    {
      Serial.read();
    }
    Serial.print("Reg 0x01"); serialPrintBinary(rf69.spiRead(RH_RF69_REG_01_OPMODE));Serial.println();
    Serial.print("Reg 0x02"); serialPrintBinary(rf69.spiRead(RH_RF69_REG_02_DATAMODUL));Serial.println();
    Serial.print("Reg 0x37"); serialPrintBinary(rf69.spiRead(RH_RF69_REG_37_PACKETCONFIG1));Serial.println();
    Serial.print("Reg 0x38 "); Serial.println(rf69.spiRead(RH_RF69_REG_38_PAYLOADLENGTH));
    Serial.print("Reg 0x3b"); serialPrintBinary(rf69.spiRead(RH_RF69_REG_3B_AUTOMODES));Serial.println();
  }
  if (myAvailable())
  {
    // Should be a message for us now
    Serial.print(millis());
    Serial.print(": Rcvd: ");
    ATOMIC_BLOCK_START;
    for (int i=0; i<sizeof(rxBufferIDM); i++)
    {
      Serial.print(rxBufferIDM[i], HEX);
      Serial.print(" ");
    }
    Serial.print(" RSSI: ");
    Serial.println(lastRSSI, DEC);
    rxBufferValid = false;
    ATOMIC_BLOCK_END;
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
