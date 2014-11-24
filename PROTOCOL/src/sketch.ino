#include <SPI.h>
#include "six/six.h"
#include "../lib/bluetooth/RBL_nRF8001.h"

char RAW_PACKET[100];
size_t PACKET_LEN;
bool NEWLINE;

environment_t ENV;


void setup()
{
  NEWLINE = 0;
  PACKET_LEN = 0;

  ENV.MAJOR = 0;
  ENV.MINOR = 1;
  
  // init. and start BLE library.
  ble_begin();
  
  // init serial debug
  Serial.begin(57600);
}

int append ( char c ) {
  if ( PACKET_LEN < sizeof ( RAW_PACKET ) ) {
    RAW_PACKET[PACKET_LEN] = c;
    PACKET_LEN++;

    return 0;
  }
  else {
    PACKET_LEN = 0;
    return -1;
  }
}

void loop()
{
  // Bluetooth
  // if new RX data available
  if ( ble_available() )
  {
    while  ( ble_available() ) {
      char c = (char)ble_read();

      if ( c == '\n' ) {
        if ( NEWLINE ) {
          if ( append( '\0' ) == 0 ) {
            if ( parse_command ( RAW_PACKET, &PACKET_LEN, &ENV ) == 0 ) {
              Serial.println("OK");
            }
            else {
              Serial.println("ERROR parsing command");
            } 
          }
        }
        else
          NEWLINE = 1;
      }
      else {
        append ( c );
      }
    }
  }

  //parse_command ( "GETV 0 SIX/0.1", 14, &env );
  ble_do_events();
}


void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char c = (char)Serial.read();
    Serial.write(c);
  }
}
