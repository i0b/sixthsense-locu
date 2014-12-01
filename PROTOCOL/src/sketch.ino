#include <SPI.h>
#include <SoftPWM.h>
#include <six.h>
#include "../lib/bluetooth/RBL_nRF8001.h"

char RAW_PACKET[100];
size_t PACKET_LEN;
bool NEWLINE;
six_request_packet_t REQ_PACKET;

environment_t ENV;


void off() {
  ENV.MODE = environment_t::OFF;

  SoftPWMSetPercent(4, 0);
  SoftPWMSetPercent(5, 0);
  SoftPWMSetPercent(6, 0);
  SoftPWMSetPercent(7, 0);
  
  analogWrite(ENV.PWM_A, 0);
  analogWrite(ENV.PWM_B, 0);
}

void command(char c) {
    if (c == 'b') {
      ENV.MODE = environment_t::HEARTBEAT;
      Serial.println("set mode: heartbeat");
    }
    
    else if (c == 'h') {
      analogWrite(ENV.PWM_A, 255);
      analogWrite(ENV.PWM_B, 255);
      
      digitalWrite(ENV.DIR_A, HIGH);
      digitalWrite(ENV.DIR_B, HIGH);
      
      Serial.println("H-bridge: heat");
    }
    
    else if (c == 'c') {
      analogWrite(ENV.PWM_A, 255);
      analogWrite(ENV.PWM_B, 255);
      
      digitalWrite(ENV.DIR_A, LOW);
      digitalWrite(ENV.DIR_B, LOW);
      
      Serial.println("H-bridge: cool");
    }
    
    else if (c == 's') {
      ENV.HEARTBEAT_DELAY_PAUSE = 900;
      Serial.println("slow beat");
    } 
    else if (c == 'f') {
      ENV.HEARTBEAT_DELAY_PAUSE = 700;
      Serial.println("fast beat");
    }
    else if (c == 'v') {
      ENV.MODE = environment_t::VIBRATION;
      Serial.println("set mode: vibration");
    }

    else if (c == '1') {
      ENV.VIBRATION_LEVEL = 1;
      Serial.println("set vibration value: 1");
    }
    else if (c == '2') {
      ENV.VIBRATION_LEVEL = 2;
      Serial.println("set vibration value: 2");
    }
    else if (c == '3') {
      ENV.VIBRATION_LEVEL = 3;
      Serial.println("set vibration value: 3");
    }
    else if (c == '4') {
      ENV.VIBRATION_LEVEL = 4;
      Serial.println("set vibration value: 4");
    }
    else if (c == '5') {
      ENV.VIBRATION_LEVEL = 5;
      Serial.println("set vibration value: 5");
    }
    else if (c == '6') {
      ENV.VIBRATION_LEVEL = 6;
      Serial.println("set vibration value: 6");
    }
    else if (c == '7') {
      ENV.VIBRATION_LEVEL = 7;
      Serial.println("set vibration value: 7");
    }
    else if (c == '8') {
      ENV.VIBRATION_LEVEL = 8;
      Serial.println("set vibration value: 8");
    }
    else if (c == '9') {
      ENV.VIBRATION_LEVEL = 9;
      Serial.println("set vibration value: 9");
    }
    else if (c == '0') {
      ENV.VIBRATION_LEVEL = 0;
      Serial.println("set vibration value: 0");
    }

    else if (c == 'r') {
      ENV.MODE = environment_t::ROTATION;
      Serial.println("set mode: rotate");
    }
    else if (c == 'd') {
      ENV.DIR++;
      ENV.DIR %= 2;
      
    Serial.println("changed direction");
    }
    else if (c == '+') {
      ENV.ROTATION_SPEED += 20;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set speed to: %d\r\n", ENV.ROTATION_SPEED);
    Serial.write(buf);
    }
    else if (c == '-') {
      if ( ENV.ROTATION_SPEED >= 20 )
        ENV.ROTATION_SPEED -= 20;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set speed to: %d\r\n", ENV.ROTATION_SPEED);
    Serial.write(buf);
    }
    else if (c == '>') {
      ENV.HEARTBEAT_DELAY_PAUSE += 20;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set interval to: %d\r\n", ENV.HEARTBEAT_DELAY_PAUSE);
    Serial.write(buf);
    }
    else if (c == '<') {
      if ( ENV.HEARTBEAT_DELAY_PAUSE >= 20 )
        ENV.HEARTBEAT_DELAY_PAUSE -= 20;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set interval to: %d\r\n", ENV.HEARTBEAT_DELAY_PAUSE);
    Serial.write(buf);
    }
    else if (c == 'o') {
      off();
  
    Serial.println("off");
    }
}

void heartbeat() {
  byte motor;

  for ( motor = 0; motor < 4; motor++ )
    //if ( OUTPIN[motor] )
      SoftPWMSetPercent(motor+4, ENV.VIBRATION_LEVEL*10);
  
  delay(ENV.HEARTBEAT_DELAY_ON);

  for ( motor = 0; motor < 4; motor++ )
    //if ( OUTPIN[motor] )
      SoftPWMSetPercent(motor+4, 0);
      
  delay(ENV.HEARTBEAT_DELAY_OFF);
  
  for ( motor = 0; motor < 4; motor++ )
    //if ( OUTPIN[motor] )
      SoftPWMSetPercent(motor+4, ENV.VIBRATION_LEVEL*10);

  delay(ENV.HEARTBEAT_DELAY_ON);
      
  for ( motor = 0; motor < 4; motor++ )
    //if ( OUTPIN[motor] )
      SoftPWMSetPercent(motor+4, 0);
  
  delay(ENV.HEARTBEAT_DELAY_PAUSE);
}

void rotate() {
  byte motor;
  
  if ( ENV.DIR )
  {
    for ( motor = 0; motor < 4; motor++ )
    {
      SoftPWMSetPercent(motor+4, ENV.VIBRATION_LEVEL*10);
      delay(ENV.ROTATION_SPEED);
      SoftPWMSetPercent(motor+4, 0);
    }
  }
  
  else
  {
    for ( motor = 0; motor < 4; motor++ )
    {
      SoftPWMSetPercent(7-motor, ENV.VIBRATION_LEVEL*10);
      delay(ENV.ROTATION_SPEED);
      SoftPWMSetPercent(7-motor, 0);
    }
  }
}

void vibrate ()
{
  byte motor;

  for ( motor = 0; motor < 4; motor++ )
    SoftPWMSetPercent(motor+4, ENV.VIBRATION_LEVEL*10);
}

void setup()
{
  // Default pins set to 9 and 8 for REQN and RDYN
  // Set your REQN and RDYN here before ble_begin() if you need
  //ble_set_pins(3, 2);
  
  // Set your BLE Shield name here, max. length 10
  //ble_set_name("My Name");
  
  // init. and start BLE library.
  ble_begin();
  
  
  // init darlington array
//  pinMode(4, OUTPUT);
//  pinMode(5, OUTPUT);
//  pinMode(6, OUTPUT);
//  pinMode(7, OUTPUT);

  SoftPWMSet(4, 0);
  SoftPWMSet(5, 0);
  SoftPWMSet(6, 0);
  SoftPWMSet(7, 0);
  
  // init rotation values
  ENV.DIR = 1;
  ENV.ROTATION_SPEED = 100;
  
  // init heartbeat values
  ENV.HEARTBEAT_DELAY_ON = 100;
  ENV.HEARTBEAT_DELAY_OFF = 70;
  ENV.HEARTBEAT_DELAY_PAUSE = 800;
  
  // init H-bridge
  ENV.PWM_A =  3;
  ENV.PWM_B = 11;
  ENV.DIR_A = 12;
  ENV.DIR_B = 13;
  
  pinMode(ENV.PWM_A, OUTPUT);
  pinMode(ENV.PWM_B, OUTPUT);
  pinMode(ENV.DIR_A, OUTPUT);
  pinMode(ENV.DIR_B, OUTPUT);

  // init software pwm module
  SoftPWMBegin();
 
  // set outputs off
  off();
 
  // set environment 
  NEWLINE = 0;
  PACKET_LEN = 0;

  ENV.VERSION_MAJOR = 0;
  ENV.VERSION_MINOR = 1;
  
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
  if ( ENV.MODE == environment_t::HEARTBEAT )
    heartbeat();
    
  else if ( ENV.MODE == environment_t::ROTATION )
    rotate();
    
  else if ( ENV.MODE == environment_t::VIBRATION )
    vibrate();
  
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
            
            char buf[50];
            snprintf(buf, sizeof(buf), "Received package: %s\r\n", RAW_PACKET);
            Serial.write(buf);

            if ( parse_command ( RAW_PACKET, &PACKET_LEN, &REQ_PACKET ) == 0 ) {
              Serial.println("VALIDE package received");
              if ( eval_command ( &REQ_PACKET, &ENV ) == 0 ) {
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
