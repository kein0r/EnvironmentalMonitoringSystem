#include <ioCC2530.h>
#include "PlatformTypes.h"
#include <board.h>
#include <dht22.h>

int main( void )
{
  /** TODO: needs to be changed to only alter bit by bit */
  P0DIR = HAL_PINOUTPUT;
  /* switch on sensor (P0.0 connected to GND, P0.1 connected to VCC) */
  P0_0 = HAL_LOW;
  P0_1 = HAL_HIGH;
  ledInit();
  while(1)
  {
    ledOn();
    DHT22_readValues();
    ledOff();
    delay_us(10000);
  }
  return 0;
}
