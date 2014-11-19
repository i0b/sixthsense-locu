#include <SPI.h>
#include <SoftPWM.h>
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

byte VIBRATION_VAL;


void off() {
  MODE = 'o';
  SoftPWMSetPercent(4, 0);
  SoftPWMSetPercent(5, 0);
  SoftPWMSetPercent(6, 0);
  SoftPWMSetPercent(7, 0);
  
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
      DELAY_PAUSE = 900;
      Serial.write("slow beat\n");
    } 
    else if (c == 'f') {
      DELAY_PAUSE = 700;
      Serial.write("fast beat\n");
    }
    else if (c == 'v') {
      MODE = 'v';
      Serial.write("set mode: vibration\n");
    }

    else if (c == '1') {
      //OUTPIN[0]++;
      //  OUTPIN[0] %= 2;
      //Serial.write("toggle output: 1\n");
      VIBRATION_VAL = 1;
      Serial.write("set vibration value: 1\n");
    }
    else if (c == '2') {
    //    OUTPIN[1]++;
    //      OUTPIN[1] %= 2;
    //    Serial.write("toggle output: 2\n");
      VIBRATION_VAL = 2;
      Serial.write("set vibration value: 2\n");
    }
    else if (c == '3') {
    //    OUTPIN[2]++;
    //      OUTPIN[2] %= 2;
    //    Serial.write("toggle output: 3\n");
      VIBRATION_VAL = 3;
      Serial.write("set vibration value: 3\n");
    }
    else if (c == '4') {
      //    OUTPIN[3]++;
      //      OUTPIN[3] %= 2;
      //    Serial.write("toggle output: 4\n");
      VIBRATION_VAL = 4;
      Serial.write("set vibration value: 4\n");
    }
    else if (c == '5') {
      VIBRATION_VAL = 5;
      Serial.write("set vibration value: 5\n");
    }
    else if (c == '6') {
      VIBRATION_VAL = 6;
      Serial.write("set vibration value: 6\n");
    }
    else if (c == '7') {
      VIBRATION_VAL = 7;
      Serial.write("set vibration value: 7\n");
    }
    else if (c == '8') {
      VIBRATION_VAL = 8;
      Serial.write("set vibration value: 8\n");
    }
    else if (c == '9') {
      VIBRATION_VAL = 9;
      Serial.write("set vibration value: 9\n");
    }
    else if (c == '0') {
      VIBRATION_VAL = 0;
      Serial.write("set vibration value: 0\n");
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
      if ( ROTATION_ON_DELAY >= 20 )
        ROTATION_ON_DELAY -= 20;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set speed to: %d\n", ROTATION_ON_DELAY);
    Serial.write(buf);
    }
    else if (c == '>') {
      DELAY_PAUSE += 20;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set interval to: %d\n", DELAY_PAUSE);
    Serial.write(buf);
    }
    else if (c == '<') {
      if ( DELAY_PAUSE >= 20 )
        DELAY_PAUSE -= 20;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set interval to: %d\n", DELAY_PAUSE);
    Serial.write(buf);
    }

// BEGIN: hack for presentation
    else if (c == 'A') {
      if ( DELAY_PAUSE >= 20 )
        DELAY_PAUSE -= 20;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set interval to: %d\n", DELAY_PAUSE);
    Serial.write(buf);
    }
    else if (c == 'B') {
      if ( DELAY_PAUSE >= 50 )
        DELAY_PAUSE -= 50;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set interval to: %d\n", DELAY_PAUSE);
    Serial.write(buf);
    }
    else if (c == 'C') {
      if ( DELAY_PAUSE >= 100 )
        DELAY_PAUSE -= 100;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set interval to: %d\n", DELAY_PAUSE);
    Serial.write(buf);
    }


    else if (c == 'R') {
      DELAY_PAUSE = 800;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set interval to: %d\n", DELAY_PAUSE);
    Serial.write(buf);
    }


    else if (c == 'X') {
      DELAY_PAUSE += 100;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set interval to: %d\n", DELAY_PAUSE);
    Serial.write(buf);
    }
    else if (c == 'Y') {
      DELAY_PAUSE += 50;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set interval to: %d\n", DELAY_PAUSE);
    Serial.write(buf);
    }
    else if (c == 'Z') {
      DELAY_PAUSE += 20;
      
      char buf[50];
      snprintf(buf, sizeof(buf), "set interval to: %d\n", DELAY_PAUSE);
    Serial.write(buf);
    }
// END: hack for presentation

    else if (c == 'o') {
      off();
  
    Serial.write("off\n");
    }
}

void heartbeat() {
  byte motor;

  for ( motor = 0; motor < 4; motor++ )
    //if ( OUTPIN[motor] )
      SoftPWMSetPercent(motor+4, VIBRATION_VAL*10);
  
  delay(DELAY_ON);

  for ( motor = 0; motor < 4; motor++ )
    //if ( OUTPIN[motor] )
      SoftPWMSetPercent(motor+4, 0);
      
  delay(DELAY_OFF);
  
  for ( motor = 0; motor < 4; motor++ )
    //if ( OUTPIN[motor] )
      SoftPWMSetPercent(motor+4, VIBRATION_VAL*10);

  delay(DELAY_ON);
      
  for ( motor = 0; motor < 4; motor++ )
    //if ( OUTPIN[motor] )
      SoftPWMSetPercent(motor+4, 0);
  
  delay(DELAY_PAUSE);
}

void rotate() {
  byte motor;
  
  if ( DIR )
  {
    for ( motor = 0; motor < 4; motor++ )
    {
      SoftPWMSetPercent(motor+4, VIBRATION_VAL*10);
      delay(ROTATION_ON_DELAY);
      SoftPWMSetPercent(motor+4, 0);
    }
  }
  
  else
  {
    for ( motor = 0; motor < 4; motor++ )
    {
      SoftPWMSetPercent(7-motor, VIBRATION_VAL*10);
      delay(ROTATION_ON_DELAY);
      SoftPWMSetPercent(7-motor, 0);
    }
  }
}

void vibrate ()
{
  byte motor;

  for ( motor = 0; motor < 4; motor++ )
    SoftPWMSetPercent(motor+4, VIBRATION_VAL*10);
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
  
  byte motor;
  for ( motor = 0; motor < 4; motor++ )
    OUTPIN[motor] = 0;
  
  // init rotation values
  DIR = 1;
  ROTATION_ON_DELAY = 100;
  
  // init heartbeat values
  DELAY_ON = 100;
  DELAY_OFF = 70;
  DELAY_PAUSE = 800;
  
  // init H-bridge
  PWM_A =  3;
  PWM_B = 11;
  DIR_A = 12;
  DIR_B = 13;
  
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);

  // init software pwm module
  SoftPWMBegin();
 
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
    
  else if (MODE == 'v')
    vibrate();
  
  // Bluetooth
  // if new RX data available
  if ( ble_available() )
  {
    while  ( ble_available() ) {
      char c = (char)ble_read();
      command(c);
    
      //char buf[50];
      //snprintf(buf, sizeof(buf), "Received command: %c\n", c);
      //Serial.write(buf);
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
