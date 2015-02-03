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
  PACKET_LEN = strlen ( command );
  Serial.println(command);

  six::parse_command ( command, &PACKET_LEN, &REQUEST_PACKET, &RESPONSE_PACKET );
  six::evaluate_command ( &REQUEST_PACKET, &RESPONSE_PACKET );

Serial.println("OK");
}

void command(char c) {
// HACK
    if (c == 'o') {
      execute_command ( "SM 0 OFF SIX/0.1" );
      execute_command ( "SM 1 OFF SIX/0.1" );
      execute_command ( "SM 2 OFF SIX/0.1" );
    }
    else if ( c >= '0' && c <= '9' ) {
      char command[20];

      // vibration
      byte percent =  map ( c, '0', '9', 0, 100 );
      snprintf ( command, 20, "SI 0 %d SIX/0.1", percent );
      execute_command ( command );
      
/*
      // peltier
      byte on_off =  map ( c, '0', '9', 0, 2 );
      snprintf ( command, 20, "SI 1 %d SIX/0.1", on_off );
      execute_command ( command );

*/
      delay ( 200 );
      // electro
      snprintf ( command, 20, "SI 2 %c SIX/0.1", c );
      execute_command ( command );
    }
    else if (c == 'e') {
      execute_command ( "SM 2 ELECTRO SIX/0.1" );
    }
    else if (c == 'v') {
      execute_command ( "SM 0 VIBRATION SIX/0.1" );
    }
    else if (c == 's') {
      execute_command ( "SM 0 SERVO SIX/0.1" );
    }
    else if (c == 'c') {
      //execute_command ( "SM 2 ELECTRO SIX/0.1" );

      execute_command ( "SM 1 TEMPERATURE SIX/0.1" );
      execute_command ( "SI 1 2 SIX/0.1" );
    }
    else if (c == 'h') {
      execute_command ( "SM 1 TEMPERATURE SIX/0.1" );
      execute_command ( "SI 1 1 SIX/0.1" );
    }
    else if (c == 'l') {
      execute_command ( "LIST SIX/0.1" );
    }
    else if (c == 'b') {
      execute_command ( "SM 0 HEARTBEAT SIX/0.1" );

      delay ( 200 );

      execute_command ( "SP 0 80 SIX/0.1" );
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
  BLEMini.begin ( 57600 );

  // set environment 
  PACKET_LEN = 0;
  RESPONSE_PACKET.body = PACKET_DATA;

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
  PACKET_LEN = 0;
  //six::packet_data_init ();

  //TODO wait for correct ending
  // Bluetooth
  // if new RX data available

/*
  if ( BLEMini.available() ) {
    while ( BLEMini.available() ) {
      //PACKET_DATA [ ( PACKET_LEN++ ) % REQUEST_RESPONSE_PACKET_LEN ] = (char) ble_read();
      char c = (char) BLEMini.read();
      //six::packet_data_append ( c );
      //append ( c );
      Serial.print(c);
    }
*/

  //if ( PACKET_LEN > 3 && PACKET_DATA [ PACKET_LEN-4 ] == '\r'
  //                    && PACKET_DATA [ PACKET_LEN-3 ] == '\n'
  //                    && PACKET_DATA [ PACKET_LEN-2 ] == '\r'
  //                    && PACKET_DATA [ PACKET_LEN-1 ] == '\n' ) {

  if ( PACKET_LEN > 3 ) {
    six::parse_command ( PACKET_DATA, &PACKET_LEN, &REQUEST_PACKET, &RESPONSE_PACKET );
    six::evaluate_command ( &REQUEST_PACKET, &RESPONSE_PACKET );
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

void serialEvent() {
  while ( Serial.available() ) {
    char c = (char) Serial.read();
    command ( c );
  }
}
