#include <ioCC2530.h>
#include "PlatformTypes.h"
#include <board.h>
#include <dht22.h>

int main( void )
{
  volatile DHT22State_t DHT22State;
  int i;
  Board_init();
  P0DIR_0 = HAL_PINOUTPUT;
  P0DIR_2 = HAL_PINOUTPUT;
  P0DIR_4 = HAL_PINOUTPUT;
  ledInit();
  DHT22_init();
  while(1)
  {
    ledOn();
    DHT22State = DHT22_readValues();
    DHT22State++;
    DHT22_init();
    ledOff();
    delay_us(30000);
    delay_us(30000);
    delay_us(30000);
  }
  return 0;
}
