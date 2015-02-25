#include <SPI.h>
#include <six.h>
#include <execute.h>
//#include <RBL_nRF8001.h>
#include <ble_shield.h>


char PACKET_DATA [ REQUEST_RESPONSE_PACKET_LEN ];
size_t PACKET_LEN;

six::request_packet_t  REQUEST_PACKET;
six::response_packet_t RESPONSE_PACKET;

void execute_command ( char* command ) {
  strcpy ( PACKET_DATA, command );
  PACKET_DATA [ strlen ( command ) ] = '\0';
  PACKET_LEN = strlen ( command ) + 1;

  six::parse_command ( PACKET_DATA, &PACKET_LEN, &REQUEST_PACKET, &RESPONSE_PACKET );
  six::evaluate_command ( &REQUEST_PACKET, &RESPONSE_PACKET );
}

void command(char c) {
// HACK
    if (c == 'b') {
      execute_command ( "SM 0 BEAT SIX/0.1" );
      execute_command ( "SP 0 40 SIX/0.1" );
    }
    else if (c == 's') {
      execute_command ( "SP 0 800 SIX/0.1" );
    } 
    else if (c == 'f') {
      execute_command ( "SP 0 600 SIX/0.1" );
    }
    else if (c == 'v' ) {
      execute_command ( "SM 0 VIB SIX/0.1" );
    }
    else if (c == 'n') {
      execute_command ( "SM 0 ROT SIX/0.1" );
      execute_command ( "SP 0 30 SIX/0.1" );
    }
    else if (c == 'o') {
      char command[20];
      for ( byte uid = 0; uid < 10; uid++ ) {
        snprintf ( command, 20, "SM %d OFF SIX/0.1", uid );
        execute_command ( command );
      }
    }
    else if (c == 'a') {
      char command[20];
      for ( byte uid = 5; uid <= 8; uid++ ) {
        snprintf ( command, 20, "SI %d 90 SIX/0.1", uid );
        execute_command ( command );
        snprintf ( command, 20, "SM %d SERVO SIX/0.1", uid );
        execute_command ( command );
      }
    }

    else if ( c >= '0' && c <= '9' ) {
      //byte value =  ( ( map ( c, '0', '9', 0, 255 ) ) / 10 ) * 10;
      byte value  =  map ( c, '0', '9', 0, 100 );
      byte mapped =  map ( c, '0', '9', 0, 180 );

      char command[20];
      for ( byte uid = 5; uid <= 8; uid++ ) {
        snprintf ( command, 20, "SI %d %d SIX/0.1", uid, mapped );
        execute_command ( command );
      }

      snprintf ( command, 20, "SI 0 %d SIX/0.1",  value );
      execute_command ( command );
    }
    else if (c == 'l') {
      execute_command ( "LIST SIX/0.1" );
    }
    else if (c == 'm') {
      execute_command ( "GM 0 SIX/0.1" );
    }
    else if (c == 'i') {
      execute_command ( "GI 0 SIX/0.1" );
    }
    else if (c == 'p') {
      execute_command ( "GP 0 SIX/0.1" );
    }
    else if (c == 'q') {
      execute_command ( "SM 0 VIB SIX/0.1" );
    }
    else if (c == 'w') {
      execute_command ( "SP 0 0 SIX/0.1" );
    }
    else if (c == 'e') {
      execute_command ( "SP 0 1 SIX/0.1" );
    }
    else if (c == 'r') {
      execute_command ( "SP 0 3 SIX/0.1" );
    }
    else if (c == 't') {
      execute_command ( "SP 0 7 SIX/0.1" );
    }
    else if (c == 'y') {
      execute_command ( "SP 0 15 SIX/0.1" );
    }
    else if (c == 'c') {
      //execute_command ( "SM 2 ELEC SIX/0.1" );
      char command[20];
      for ( byte uid = 1; uid <= 4; uid++ ) {
        snprintf ( command, 20, "SM %d TEMP SIX/0.1", uid );
        execute_command ( command );

        snprintf ( command, 20, "SI %d 2 SIX/0.1", uid );
        execute_command ( command );
      }
    }
    else if (c == 'h') {
      char command[20];
      for ( byte uid = 1; uid <= 4; uid++ ) {
        snprintf ( command, 20, "SM %d TEMP SIX/0.1", uid );
        execute_command ( command );

        snprintf ( command, 20, "SI %d 1 SIX/0.1", uid );
        execute_command ( command );
      }
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
  Serial.begin(57600);
  delay(200);

  execute::init_executor();
  //delay(100);

  // Default pins set to 9 and 8 for REQN and RDYN
  // Set your REQN and RDYN here before ble_begin() if you need
  //ble_set_pins(3, 2);
  
  // Set your BLE Shield name here, max. length 10
  //ble_set_name("LOCU");
  
  // init. and start BLE library.
  ble_begin();
  //ble_connect();

 
  // set environment 
  PACKET_LEN = 0;
  RESPONSE_PACKET.body = PACKET_DATA;
}


// timer compare interrupt service routine
// calls timer_isr every 10ms
// calls servo_isr every 10us
// parameter set in init_executor
ISR(TIMER3_COMPA_vect) {
  noInterrupts();
  execute::timer_isr();
  interrupts();
}
/*
ISR(TIMER4_COMPA_vect) {
  noInterrupts();
  execute::servo_isr();
  interrupts();
}
*/

/*
void loop () {

  execute_command ( "SM 2 VIB SIX/0.1" );
  delay ( 1000 );
  execute_command ( "SP 2 1 SIX/0.1" );
  delay ( 1000 );
  execute_command ( "SP 2 3 SIX/0.1" );
  delay ( 1000 );
  execute_command ( "SP 2 7 SIX/0.1" );
  delay ( 1000 );
  execute_command ( "SP 2 15 SIX/0.1" );
  delay ( 1000 );
  execute_command ( "SM 2 OFF SIX/0.1" );
  delay ( 1000 );

  execute_command ( "SM 3 TEMP SIX/0.1" );
  execute_command ( "SI 3 2 SIX/0.1" );
  
  execute_command ( "SM 2 ROT SIX/0.1" );
  execute_command ( "SP 2 25 SIX/0.1" );
  delay ( 4000 );
  execute_command ( "SP 2 20 SIX/0.1" );
  delay ( 3000 );
  execute_command ( "SP 2 15 SIX/0.1" );
  delay ( 3000 );
  //execute_command ( "SM 2 OFF SIX/0.1" );
  //delay ( 1000 );

  execute_command ( "SM 3 TEMP SIX/0.1" );
  execute_command ( "SI 3 1 SIX/0.1" );

  execute_command ( "SM 2 BEAT SIX/0.1" );
  execute_command ( "SP 2 60 SIX/0.1" );
  delay ( 5000 );
  execute_command ( "SP 2 45 SIX/0.1" );
  delay ( 5000 );
  execute_command ( "SM 3 OFF SIX/0.1" );
  execute_command ( "SM 2 OFF SIX/0.1" );
  delay ( 1000 );

  
  execute_command ( "SM 4 SERVO SIX/0.1" );
  char command[20];
  
  for ( uint8_t value = 80; value <= 180; value = value + 20 ) {
    snprintf ( command, 20, "SI 4 %d SIX/0.1", value );

    execute_command ( command );
    delay ( 500 );
  }
  for ( uint8_t value = 180; value >= 0 && value <= 180; value = value - 20 ) {
    snprintf ( command, 20, "SI 4 %d SIX/0.1", value );

    execute_command ( command );
    delay ( 500 );
  }
  for ( uint8_t value = 0; value <= 80; value = value + 20 ) {
    snprintf ( command, 20, "SI 4 %d SIX/0.1", value );

    execute_command ( command );
    delay ( 500 );
  }

}
*/

void loop()
{
//TODO wait for correct ending
  // Bluetooth
  // if new RX data available
  if ( ble_available() ) {
    while ( ble_available() ) {
      //PACKET_DATA [ ( PACKET_LEN++ ) % REQUEST_RESPONSE_PACKET_LEN ] = (char) ble_read();
      char c = (char) ble_read();
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

  ble_do_events();
}

void serialEvent() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    command(c);
  }
}
