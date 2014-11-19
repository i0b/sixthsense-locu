#include <SPI.h>
#include "../lib/six.h"
#include "../lib/bluetooth/RBL_nRF8001.h"

void setup()
{
  // init. and start BLE library.
  ble_begin();
  
  // init serial debug
  Serial.begin(57600);
}


void loop()
{
  // Bluetooth
  // if new RX data available
  if ( ble_available() )
  {
    while  ( ble_available() ) {
      char c = (char)ble_read();
    }
  }
  
  ble_do_events();
}


void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char c = (char)Serial.read();
  }
}
