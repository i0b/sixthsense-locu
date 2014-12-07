#include <SPI.h>
#include <six.h>
#include "../lib/six/actuator.h"
#include "../lib/six/execute.h"
#include "../lib/bluetooth/RBL_nRF8001.h"

#define VIBRATION_ARRAY_UUID 0
#define VIBRATION_RING_UUID  2

char RAW_PACKET[100];
size_t PACKET_LEN;
bool NEWLINE;
six::request_packet_t REQ_PACKET;

void off() {
  for ( uint8_t uuid = 0; uuid < actuator::NUMBER_ACTUATORS; uuid++ ) {
    execute::set_mode ( uuid, execute::OFF );
  }
}

void command(char c) {
// HACK
    if (c == 'b') {
      execute::set_mode ( VIBRATION_ARRAY_UUID, execute::HEARTBEAT );
      Serial.println("set mode: heartbeat");
    }
    else if (c == 's') {
      execute::set_parameter ( VIBRATION_ARRAY_UUID, 800 );
      Serial.println("slow beat");
    } 
    else if (c == 'f') {
      execute::set_parameter ( VIBRATION_ARRAY_UUID, 600 );
      Serial.println("fast beat");
    }
    else if (c == 'v') {
      execute::set_mode ( VIBRATION_ARRAY_UUID, execute::VIBRATION );
      Serial.println("set mode: vibration");
    }
    else if (c == 'r') {
      execute::set_mode ( VIBRATION_ARRAY_UUID, execute::ROTATION );
      Serial.println("set mode: rotate");
    }
    else if (c == 'o') {
      execute::set_mode ( VIBRATION_ARRAY_UUID, execute::OFF );
      Serial.println("off");
    }
    else if ( c >= '0' && c <= '9' ) {
      byte value = map ( c, '0', '9', 0, 255 );
      execute::set_intensity ( VIBRATION_ARRAY_UUID, value );
      Serial.print("set vibration value: ");
      Serial.println(value);
    }
// HACK END
}

void setup () {
  Serial.begin(57600);
  delay(500);
  // Default pins set to 9 and 8 for REQN and RDYN
  // Set your REQN and RDYN here before ble_begin() if you need
  //ble_set_pins(3, 2);
  
  // Set your BLE Shield name here, max. length 10
  //ble_set_name("My Name");
  
  // init. and start BLE library.
  //ble_begin();

  execute::init_executor();
 
  // set environment 
  NEWLINE = 0;
  PACKET_LEN = 0;
  
  // init serial debug
}

ISR(TIMER1_COMPA_vect) {         // timer compare interrupt service routine
  execute::timer_isr();
}

// needed for creating RAW_PACKET
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
/*
  // Bluetooth
  // if new RX data available
  if ( ble_available() )
  {
    while  ( ble_available() ) {
      char c = (char)ble_read();

      if ( c == '\n' ) {
        if ( NEWLINE ) {
          if ( append( '\0' ) == 0 ) {
            NEWLINE = 0;
            PACKET_LEN = 0;
            
            Serial.print ( "Received package: " );
            Serial.println ( RAW_PACKET );

            if ( six::parse_command ( RAW_PACKET, &PACKET_LEN, &REQ_PACKET ) == 0 ) {
              Serial.println("VALIDE package received");
              if ( six::eval_command ( &REQ_PACKET ) == 0 ) {
                Serial.println("OK new settings applied");
              }
              else {
                Serial.println("ERROR applying new settings");
              }
            }
            else {
              Serial.println("INVALIDE package received");
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
*/
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char c = (char)Serial.read();
    command(c);
  }
}
