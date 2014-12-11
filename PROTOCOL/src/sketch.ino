#include <SPI.h>
#include <six.h>
#include <execute.h>
#include <RBL_nRF8001.h>

char PACKET_DATA [ REQUEST_RESPONSE_PACKET_LEN ];
size_t PACKET_LEN;

bool NEWLINE;

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
    }
    else if (c == 's') {
      execute_command ( "SP 0 800 SIX/0.1" );
    } 
    else if (c == 'f') {
      execute_command ( "SP 0 600 SIX/0.1" );
    }
    else if (c == 'v') {
      execute_command ( "SM 0 VIB SIX/0.1" );
    }
    else if (c == 'n') {
      execute_command ( "SM 2 ROT SIX/0.1" );
      execute_command ( "SP 2 30 SIX/0.1" );
    }
    else if (c == 'o') {
      execute_command ( "SM 0 OFF SIX/0.1" );
    }
    else if (c == 'a') {
      execute_command ( "SM 4 SERVO SIX/0.1" );
    }

    else if ( c >= '0' && c <= '9' ) {
      //byte value =  ( ( map ( c, '0', '9', 0, 255 ) ) / 10 ) * 10;
      byte value =  map ( c, '0', '9', 0, 255 );
      char command[20];
      snprintf ( command, 20, "SV 2 %d SIX/0.1", value );
      
      execute_command ( command );
    }
    else if (c == 'l') {
      execute_command ( "LIST SIX/0.1" );
    }
    else if (c == 'm') {
      execute_command ( "GM 0 SIX/0.1" );
    }
    else if (c == 'i') {
      execute_command ( "GV 0 SIX/0.1" );
    }
    else if (c == 'p') {
      execute_command ( "GP 0 SIX/0.1" );
    }
    else if (c == 'q') {
      execute_command ( "SM 2 SERVO SIX/0.1" );
    }
    else if (c == 'w') {
      execute_command ( "SP 2 0 SIX/0.1" );
    }
    else if (c == 'e') {
      execute_command ( "SP 2 1 SIX/0.1" );
    }
    else if (c == 'r') {
      execute_command ( "SP 2 3 SIX/0.1" );
    }
    else if (c == 't') {
      execute_command ( "SP 2 7 SIX/0.1" );
    }
    else if (c == 'y') {
      execute_command ( "SP 2 15 SIX/0.1" );
    }
// HACK END
}

// needed for creating PACKET_DATA
int append ( char c ) {
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

 
  // set environment 
  NEWLINE = 0;
  PACKET_LEN = 0;
  RESPONSE_PACKET.body = PACKET_DATA;
}


// timer compare interrupt service routine
// calls timer_isr every 10ms
// parameter set in init_executor
ISR(TIMER1_COMPA_vect) {
  noInterrupts();
  execute::timer_isr();
  interrupts();
}

void loop()
{
//TODO REPLACE REQ_PACKET, EVAL_COMMAND
  // Bluetooth
  // if new RX data available
  if ( ble_available() )
  {
    Serial.print ( "ACK\r\n" );
    while  ( ble_available() ) {
      char c = (char)ble_read();
//TODO REMOVE temporary print all character
      Serial.print ( c );

      if ( c == '\n' ) {
        if ( NEWLINE ) {
          if ( append( '\0' ) == 0 ) {
            NEWLINE = 0;
            PACKET_LEN = 0;
            
            Serial.print ( "Received package: " );
            Serial.println ( PACKET_DATA );

            six::parse_command ( PACKET_DATA, &PACKET_LEN, &REQUEST_PACKET, &RESPONSE_PACKET );
            six::evaluate_command ( &REQUEST_PACKET, &RESPONSE_PACKET );
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

  ble_do_events();
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char c = (char)Serial.read();
    command(c);
  }
}
