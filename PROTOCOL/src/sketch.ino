#include <SPI.h>
#include <six.h>
#include "../lib/six/actuator.h"
#include "../lib/six/execute.h"
#include "../lib/bluetooth/RBL_nRF8001.h"

#define VIBRATION_RING_UUID 1

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
      execute::set_mode ( VIBRATION_RING_UUID, execute::HEARTBEAT );
      Serial.println("set mode: heartbeat");
    }
    
    else if (c == 's') {
      execute::set_parameter ( VIBRATION_RING_UUID, 800 );
      Serial.println("slow beat");
    } 
    else if (c == 'f') {
      execute::set_parameter ( VIBRATION_RING_UUID, 600 );
      Serial.println("fast beat");
    }
    else if (c == 'v') {
      execute::set_mode ( VIBRATION_RING_UUID, execute::VIBRATION );
      Serial.println("set mode: vibration");
    }

    else if (c == '1') {
      execute::set_intensity ( VIBRATION_RING_UUID, map (1, 1, 10, 0, 255 ) );
      Serial.println("set vibration value: 1");
    }
    else if (c == '2') {
      execute::set_intensity ( VIBRATION_RING_UUID, map (2, 1, 10, 0, 255 ) );
      Serial.println("set vibration value: 2");
    }
    else if (c == '3') {
      execute::set_intensity ( VIBRATION_RING_UUID, map (3, 1, 10, 0, 255 ) );
      Serial.println("set vibration value: 3");
    }
    else if (c == '4') {
      execute::set_intensity ( VIBRATION_RING_UUID, map (4, 1, 10, 0, 255 ) );
      Serial.println("set vibration value: 4");
    }
    else if (c == '5') {
      execute::set_intensity ( VIBRATION_RING_UUID, map (5, 1, 10, 0, 255 ) );
      Serial.println("set vibration value: 5");
    }
    else if (c == '6') {
      execute::set_intensity ( VIBRATION_RING_UUID, map (6, 1, 10, 0, 255 ) );
      Serial.println("set vibration value: 6");
    }
    else if (c == '7') {
      execute::set_intensity ( VIBRATION_RING_UUID, map (7, 1, 10, 0, 255 ) );
      Serial.println("set vibration value: 7");
    }
    else if (c == '8') {
      execute::set_intensity ( VIBRATION_RING_UUID, map (8, 1, 10, 0, 255 ) );
      Serial.println("set vibration value: 8");
    }
    else if (c == '9') {
      execute::set_intensity ( VIBRATION_RING_UUID, map (9, 1, 10, 0, 255 ) );
      Serial.println("set vibration value: 9");
    }
    else if (c == '0') {
      execute::set_intensity ( VIBRATION_RING_UUID, map (0, 1, 10, 0, 255 ) );
      Serial.println("set vibration value: 0");
    }

    else if (c == 'r') {
      execute::set_mode ( VIBRATION_RING_UUID, execute::ROTATION );
      Serial.println("set mode: rotate");
    }

    else if (c == 'o') {
      execute::set_mode ( VIBRATION_RING_UUID, execute::OFF );
      Serial.println("off");
    }
// HACK END
}

void setup () {
  // Default pins set to 9 and 8 for REQN and RDYN
  // Set your REQN and RDYN here before ble_begin() if you need
  //ble_set_pins(3, 2);
  
  // Set your BLE Shield name here, max. length 10
  //ble_set_name("My Name");
  
  // init. and start BLE library.
  ble_begin ();

  execute::init_executor();
  
 
  // set environment 
  NEWLINE = 0;
  PACKET_LEN = 0;
  
  // init serial debug
  Serial.begin(57600);
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
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char c = (char)Serial.read();
    command(c);
  }
}
