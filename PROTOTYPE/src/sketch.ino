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

/*
void command(char c) {
// HACK
    char command[100];

    if (c == 'o') {
      for ( uint8_t uuid = 0; uuid < 3; uuid++ ) {
        snprintf ( command, sizeof(command), "SM %d OFF SIX/0.1", uuid );
        execute_command ( command );
      }
    }
    else if ( c >= '0' && c <= '9' ) {
      // vibration
      byte percent =  map ( c, '0', '9', 0, 100 );
      snprintf ( command, sizeof(command), "SI 0 %d SIX/0.1", percent );
      execute_command ( command );
    }
    else if (c == 'e') {
      snprintf ( command, sizeof(command), "SM 2 ELECTRIC SIX/0.1");
      execute_command ( command );
    }
    else if (c == 'v') {
      snprintf ( command, sizeof(command), "SM 0 VIBRATION SIX/0.1" );
      execute_command ( command );
    }
    else if (c == 's') {
      snprintf ( command, sizeof(command), "SM 3 SERVO SIX/0.1" );
      execute_command ( command );
    }
    else if (c == 'c') {
      snprintf ( command, sizeof(command), "SM 1 TEMPERATURE SIX/0.1" );
      execute_command ( command );
      snprintf ( command, sizeof(command), "SI 1 2 SIX/0.1" );
      execute_command ( command );
    }
    else if (c == 'h') {
      snprintf ( command, sizeof(command), "SM 1 TEMPERATURE SIX/0.1" );
      execute_command ( command );
      snprintf ( command, sizeof(command), "SI 1 1 SIX/0.1" );
      execute_command ( command );
    }
    else if (c == 'l') {
      snprintf ( command, sizeof(command), "LIST SIX/0.1" );
      execute_command ( command );
    }
    else if (c == 'b') {
      snprintf ( command, sizeof(command), "SM 0 HEARTBEAT SIX/0.1" );
      execute_command ( command );
      snprintf ( command, sizeof(command), "SP 0 80 SIX/0.1" );
      execute_command ( command );
    }
// HACK END
}
*/

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


void loop()
{
  //PACKET_LEN = 0;
  //six::packet_data_init ();

  //TODO wait for correct ending
  // Bluetooth
  // if new RX data available
  

//char PACKET_DATA [ REQUEST_RESPONSE_PACKET_LEN ];
//size_t PACKET_LEN;

  if ( BLEMini.available() ) {
    if ( PACKET_LEN == 0 ) {
      Serial.print("DEBUG: bluetooth_rx [ ");
    }
    while ( BLEMini.available() ) {
      //PACKET_DATA [ ( PACKET_LEN++ ) % REQUEST_RESPONSE_PACKET_LEN ] = (char) ble_read();
      char c = (char) BLEMini.read();
      switch ( append ( c ) ) {
        case -1:
          Serial.println("ERROR: bluetooth data too long");
          break;
        case 0:
          Serial.print( c );
          break;
        case 1:
          //TODO remove this hack
          for ( size_t endchar = PACKET_LEN-1; endchar >= 0 && PACKET_DATA[endchar] != '1'; endchar--) {
            PACKET_DATA[endchar] = '\0';
          }

          Serial.println(" ]");

          //Serial.print("DEBUG: parse_command [ ");
          //Serial.print(PACKET_DATA);
          //Serial.println(" ]");
          if ( six::parse_command ( PACKET_DATA, &PACKET_LEN, &REQUEST_PACKET, &RESPONSE_PACKET ) == 0 ) {
            six::evaluate_command ( &REQUEST_PACKET, &RESPONSE_PACKET );
          }
          PACKET_LEN = 0;
          break;
      }
    }
  }
/*
  }
  while (1) {
    execute_command ("SM 0 VIB SIX/0.1");
    delay(1000);
    execute_command ("SI 0 10 SIX/0.1");
    adafruit::setPERCENT ( 0x40, 15, 10 );
    execute::run_executor();
    delay(1000);
    execute_command ("SI 0 20 SIX/0.1");
    adafruit::setPERCENT ( 0x40, 15, 20 );
    execute::run_executor();
    delay(1000);
    execute_command ("SI 0 30 SIX/0.1");
    adafruit::setPERCENT ( 0x40, 15, 30 );
    execute::run_executor();
    delay(1000);
    execute_command ("SI 0 40 SIX/0.1");
    adafruit::setPERCENT ( 0x40, 15, 40 );
    execute::run_executor();
    delay(1000);
    execute_command ("SI 0 50 SIX/0.1");
    adafruit::setPERCENT ( 0x40, 15, 50 );
    execute::run_executor();
    delay(1000);
    execute_command ("SI 0 60 SIX/0.1");
    adafruit::setPERCENT ( 0x40, 15, 60 );
    execute::run_executor();
    delay(1000);
    execute_command ("SI 0 70 SIX/0.1");
    adafruit::setPERCENT ( 0x40, 15, 70 );
    execute::run_executor();
    delay(1000);
    execute_command ("SI 0 80 SIX/0.1");
    adafruit::setPERCENT ( 0x40, 15, 80 );
    execute::run_executor();
    delay(1000);
    execute_command ("SI 0 90 SIX/0.1");
    adafruit::setPERCENT ( 0x40, 15, 90 );
    execute::run_executor();
    delay(1000);
    execute_command ("SI 0 100 SIX/0.1");
    adafruit::setPERCENT ( 0x40, 15, 100 );
    execute::run_executor();
    delay(10000);
    execute_command ("SI 0 0 SIX/0.1");
    adafruit::setPERCENT ( 0x40, 15, 0 );
    execute::run_executor();
    delay(10000);

    Serial.println("Repeat....");
  }
*/
  execute::run_executor();
}

char cmd[256];
uint8_t cmd_pos = 0;

void serialEvent() {
/*
  while ( Serial.available() ) {
    char c = (char) Serial.read();
    command ( c );
  }
*/
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
