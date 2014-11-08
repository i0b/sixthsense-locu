#include <SPI.h>
#include "../lib/bluetooth/RBL_nRF8001.h"

int DELAY_ON;
int DELAY_OFF;
int DELAY_PAUSE;

int ROTATION_ON_DELAY;
byte DIR;

byte OUTPIN[4];
char MODE;


byte PWM_A;
byte PWM_B;
byte DIR_A;
byte DIR_B;


void off() {
  MODE = 'o';
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}

void command(char c) {
    if (c == 'b') {
    MODE = 'b';
    Serial.write("set mode: heartbeat\n");
    }
    
    else if (c == 'h') {
      analogWrite(PWM_A, 255);
      analogWrite(PWM_B, 255);
      
      digitalWrite(DIR_A, HIGH);
      digitalWrite(DIR_B, HIGH);
      
    Serial.write("H-bridge: heat\n");
    }
    
    else if (c == 'c') {
      analogWrite(PWM_A, 255);
      analogWrite(PWM_B, 255);
      
      digitalWrite(DIR_A, LOW);
      digitalWrite(DIR_B, LOW);
      
    Serial.write("H-bridge: cool\n");
    }
    
    else if (c == 's') {
    DELAY_ON = 30;
    DELAY_OFF = 90;
    DELAY_PAUSE = 900;
    Serial.write("slow beat\n");
    } 
    else if (c == 'f') {
    DELAY_ON = 30;
    DELAY_OFF = 70;
    DELAY_PAUSE = 700;
    Serial.write("fast beat\n");
    }
    else if (c == '1') {
    OUTPIN[0]++;
      OUTPIN[0] %= 2;
    Serial.write("toggle output: 1\n");
    }
    else if (c == '2') {
    OUTPIN[1]++;
      OUTPIN[1] %= 2;
    Serial.write("toggle output: 2\n");
    }
    else if (c == '3') {
    OUTPIN[2]++;
      OUTPIN[2] %= 2;
    Serial.write("toggle output: 3\n");
    }
    else if (c == '4') {
    OUTPIN[3]++;
      OUTPIN[3] %= 2;
    Serial.write("toggle output: 4\n");
    }
    else if (c == 'r') {
    MODE = 'r';
    Serial.write("set mode: rotate\n");
    }
    else if (c == 'd') {
      DIR++;
      DIR %= 2;
      
    Serial.write("changed direction\n");
    }
    else if (c == '+') {
      ROTATION_ON_DELAY += 20;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set speed to: %d\n", ROTATION_ON_DELAY);
    Serial.write(buf);
    }
    else if (c == '-') {
      ROTATION_ON_DELAY -= 20;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set speed to: %d\n", ROTATION_ON_DELAY);
    Serial.write(buf);
    }
    else if (c == 'o') {
      off();
  
    Serial.write("off\n");
    }
}

void heartbeat() {
  byte motor;
  
  for ( motor = 0; motor < 4; motor++ )
    if ( OUTPIN[motor] )
      digitalWrite(motor+4, HIGH);
  
  delay(DELAY_ON);

  for ( motor = 0; motor < 4; motor++ )
    if ( OUTPIN[motor] )
      digitalWrite(motor+4, LOW);
      
  delay(DELAY_OFF);

  for ( motor = 0; motor < 4; motor++ )
    if ( OUTPIN[motor] )
      digitalWrite(motor+4, HIGH);

  delay(DELAY_ON);
      
  for ( motor = 0; motor < 4; motor++ )
    if ( OUTPIN[motor] )
      digitalWrite(motor+4, LOW);
  
  
  delay(DELAY_PAUSE);
}

void rotate() {
  byte motor;
  
  if ( DIR )
  {
    for ( motor = 0; motor < 4; motor++ )
    {
      digitalWrite(motor+4, HIGH);
      delay(ROTATION_ON_DELAY);
      digitalWrite(motor+4, LOW);
    }
  }
  
  else
  {
    for ( motor = 0; motor < 4; motor++ )
    {
      digitalWrite(7-motor, HIGH);
      delay(ROTATION_ON_DELAY);
      digitalWrite(7-motor, LOW);
    }
  }
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
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  
  byte motor;
  for ( motor = 0; motor < 4; motor++ )
    OUTPIN[motor] = 0;
  
  // init rotation values
  DIR = 1;
  ROTATION_ON_DELAY = 100;
  
  // init heartbeat values
  DELAY_ON = 0;
  DELAY_OFF = 0;
  DELAY_PAUSE = 1000;
  
  // init H-bridge
  PWM_A =  3;
  PWM_B = 11;
  DIR_A = 12;
  DIR_B = 13;
  
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  
  
  // set outputs off
  off();
  
  // init serial debug
  Serial.begin(57600);
}


void loop()
{
  if (MODE == 'b')
    heartbeat();
    
  else if (MODE == 'r')
    rotate();
    
  
  // Bluetooth
  // if new RX data available
  if ( ble_available() )
  {
    while  ( ble_available() ) {
      char c = (char)ble_read();
      command(c);
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
