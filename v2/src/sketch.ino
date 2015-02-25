#include <SPI.h>
#include <six.h>
#include <execute.h>

//#include <adafruit.h>
//#include <string.h>


char PACKET_DATA [ REQUEST_RESPONSE_PACKET_LEN ];
size_t PACKET_LEN;

six::request_packet_t  REQUEST_PACKET;
six::response_packet_t RESPONSE_PACKET;

void execute_command ( char* command ) {
  Serial.print("DEBUG: execute_command( \"");
  Serial.print(command);
  Serial.println("\" )");

  PACKET_LEN = strlen ( command );

  six::parse_command ( command, &PACKET_LEN, &REQUEST_PACKET, &RESPONSE_PACKET );
  six::evaluate_command ( &REQUEST_PACKET, &RESPONSE_PACKET );
}

// needed for creating PACKET_DATA
int append ( char c ) {
  if ( c == '\r' || c == '\n' ) {
    return 1;
  }
  //PACKET_DATA [ ( PACKET_LEN++ ) % REQUEST_RESPONSE_PACKET_LEN ] = (char) ble_read();
  if ( PACKET_LEN+1 < REQUEST_RESPONSE_PACKET_LEN ) {
    PACKET_DATA [ PACKET_LEN ] = c;
    PACKET_LEN++;
    PACKET_DATA [ PACKET_LEN ] = '\0';

    return 0;
  }
  else {
    PACKET_LEN = 0;
    return -1;
  }
}


void fetch_and_execute_bluetooth_data() {
  bool do_execute = false;

  if ( PACKET_LEN == 0 ) {
    Serial.print("DEBUG: bluetooth_rx [ ");
  }

  while ( BLEMini.available() ) {
    char c = (char) BLEMini.read();

    switch ( append ( c ) ) {
      case -1:
        Serial.println("ERROR: bluetooth data too long");
        break;
      case  0:
        Serial.print( c );
        break;
      case  1:
        Serial.println(" ]");

        /*
        //TODO remove this hack
        for ( size_t endchar = PACKET_LEN-1; endchar >= 0 && PACKET_DATA[endchar] != '1'; endchar--) {
          PACKET_DATA[endchar] = '\0';
        }
        */

        /*
        Serial.print("DEBUG: parse_command [ ");
        Serial.print(PACKET_DATA);
        Serial.println(" ]");
        */
        do_execute = true;
        break;
      default:
        Serial.println("ERROR: unknown parsing error");
    }

    if ( do_execute ) {
      break;
    }
  }

  if ( do_execute ) {
    if ( PACKET_LEN > 3 ) {
      if ( six::parse_command ( PACKET_DATA, &PACKET_LEN, &REQUEST_PACKET, &RESPONSE_PACKET ) == 0 ) {
        six::evaluate_command ( &REQUEST_PACKET, &RESPONSE_PACKET );
      }
    }

    PACKET_LEN = 0;
  }
}


void setup () {
  // init serial debug
  Serial.begin  ( 57600 );
  BLEMini.begin ( 57600 );

  // set environment 
  PACKET_LEN = 0;
  RESPONSE_PACKET.body = PACKET_DATA;

  Serial.println("DEBUG: setup()");
  execute::init_executor();
}

// timer compare interrupt service routine
// calls timer_isr every 10ms
// parameter set in init_executor
ISR ( TIMER3_COMPA_vect ) {
  noInterrupts();
  execute::timer_isr();
  interrupts();
}
/*
ISR ( TIMER1_COMPA_vect ) {
  noInterrupts();
  execute::timer_isr();
  interrupts();
}
*/

void loop() {

  execute::run_executor();

  //six::packet_data_init ();

  //TODO wait for correct ending
  // Bluetooth
  // if new RX data available
  

//char PACKET_DATA [ REQUEST_RESPONSE_PACKET_LEN ];
//size_t PACKET_LEN;

  if ( BLEMini.available() ) {
    fetch_and_execute_bluetooth_data();
  }

}

char cmd[256];
uint8_t cmd_pos = 0;

void serialEvent() {
  while ( Serial.available() ) {
    char c = (char) Serial.read();
    if ( c == '\r' || c == '\n' ) {
      cmd[cmd_pos] = '\0';
      cmd_pos = 0;
      Serial.println();
      execute_command ( cmd );
      break;
    }
    else {
      cmd[cmd_pos] = c;
      Serial.print(c);
    }
    cmd_pos++;
  }

}
