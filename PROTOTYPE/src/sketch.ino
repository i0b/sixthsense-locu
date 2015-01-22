#include <SPI.h>
#include <six.h>
#include <execute.h>


#include <adafruit.h>

#include <Arduino.h>
//#include <AltSoftSerial.h>

/*
// For UNO, AltSoftSerial library is required, please get it from:
// http://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)  
  AltSoftSerial BLEMini;
#else
  #define BLEMini Serial1
#endif
*/

char PACKET_DATA [ REQUEST_RESPONSE_PACKET_LEN ];
size_t PACKET_LEN;

six::request_packet_t  REQUEST_PACKET;
six::response_packet_t RESPONSE_PACKET;

void execute_command ( char* command ) {
  Serial.println ( "start parsing.." );
  strcpy ( PACKET_DATA, command );
  PACKET_DATA [ strlen ( command ) ] = '\0';
  PACKET_LEN = strlen ( command ) + 1;
  Serial.println(PACKET_DATA);

  six::parse_command ( PACKET_DATA, &PACKET_LEN, &REQUEST_PACKET, &RESPONSE_PACKET );
  six::evaluate_command ( &REQUEST_PACKET, &RESPONSE_PACKET );
}

void command(char c) {
// HACK
    if (c == 'b') {
    }
    else if (c == 'o') {
      execute_command ( "SM 0 OFF SIX/0.1" );
    }
    else if ( c >= '0' && c <= '9' ) {
      byte value =  map ( c, '0', '9', 0, 100 );
      //adafruit::setPERCENT(0x40, 15, value);
      //Serial.println(value);
      char command[20];
      snprintf ( command, 20, "SI 0 %d SIX/0.1", value );
      
      execute_command ( command );
    }
    else if (c == 'v') {
      execute_command ( "SM 0 VIB SIX/0.1" );
    }
    else if (c == 'c') {
      //execute_command ( "SM 2 ELEC SIX/0.1" );

      execute_command ( "SM 3 TEMP SIX/0.1" );
      execute_command ( "SI 3 2 SIX/0.1" );
    }
    else if (c == 'h') {
      execute_command ( "SM 3 TEMP SIX/0.1" );
      execute_command ( "SI 3 1 SIX/0.1" );
    }
// HACK END
}

// needed for creating PACKET_DATA
int append ( char c ) {
  //PACKET_DATA [ ( PACKET_LEN++ ) % REQUEST_RESPONSE_PACKET_LEN ] = (char) ble_read();
  if ( PACKET_LEN < REQUEST_RESPONSE_PACKET_LEN ) {
    PACKET_DATA [ PACKET_LEN ] = c;
    PACKET_LEN++;

    return 0;
  }
  else {
    PACKET_LEN = 0;
    return -1;
  }
}

void setup () {
  // init serial debug
  Serial.begin  ( 57600 );
  //BLEMini.begin ( 57600 );

  while (!Serial){;}

  // set environment 
  PACKET_LEN = 0;
  RESPONSE_PACKET.body = PACKET_DATA;

  execute::init_executor();
}

// timer compare interrupt service routine
// calls timer_isr every 10ms
// parameter set in init_executor
/*
ISR ( TIMER3_COMPA_vect ) {
  noInterrupts();
  execute::timer_isr();
  interrupts();
}
*/
/*
ISR ( TIMER1_COMPA_vect ) {
  noInterrupts();
  execute::timer_isr();
  interrupts();
}
*/


void loop()
{
/*
//TODO wait for correct ending
  // Bluetooth
  // if new RX data available
  if ( BLEMini.available() ) {
    while ( BLEMini.available() ) {
      //PACKET_DATA [ ( PACKET_LEN++ ) % REQUEST_RESPONSE_PACKET_LEN ] = (char) ble_read();
      char c = (char) BLEMini.read();
      append ( c );
    }

  //if ( PACKET_LEN > 3 && PACKET_DATA [ PACKET_LEN-4 ] == '\r'
  //                    && PACKET_DATA [ PACKET_LEN-3 ] == '\n'
  //                    && PACKET_DATA [ PACKET_LEN-2 ] == '\r'
  //                    && PACKET_DATA [ PACKET_LEN-1 ] == '\n' ) {
    six::parse_command ( PACKET_DATA, &PACKET_LEN, &REQUEST_PACKET, &RESPONSE_PACKET );
    six::evaluate_command ( &REQUEST_PACKET, &RESPONSE_PACKET );

    PACKET_LEN = 0;
  }
*/
  delay(100);
}

void serialEvent() {
  while ( Serial.available() ) {
    char c = (char) Serial.read();
    command ( c );
  }
}
