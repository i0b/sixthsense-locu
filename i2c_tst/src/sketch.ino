#include <adafruit.h>
#include <AltSoftSerial.h>
#include <Arduino.h>
#include <Wire.h>

AltSoftSerial BLEMini;  

unsigned char ACK[6] = "ACK\r\n";

void command (char c) {
  Serial.println ( "command method called" );

  if ( c >= '0' && c <= '9') {
    Serial.print ( "PWM15 set to [" );
    BLEMini.print ( "PWM15 set to [" );
      
      uint16_t val = map ( c, '0', '9', 0, 100 );

      adafruit::setPERCENT(0x40, 15, val);
      Serial.print ( val );
      BLEMini.print ( val );
      
      Serial.println ( "]\r\n" );
      BLEMini.println ( "]\r\n" );
  }
}

void setup()
{
  Serial.begin( 57600 );
  BLEMini.begin(57600); 
  
  Wire.begin();
  adafruit::resetPWM ( 0x40 );
  adafruit::setPWMFreq( 0x40, 100 );

  // save I2C bitrate
  //uint8_t twbrbackup = TWBR;
  // must be changed after calling Wire.begin() (inside pwm.begin())
  //TWBR = 12; // upgrade to 400KHz!
  
  while(!Serial) {}
  
  Serial.println("init [done]");
}

void loop()
{
/*  
  if ( BLEMini.available() ) {
      delay(5);
    
      while ( BLEMini.available() ) {
        Serial.println ( "received data via bluetooth");
        char c = (char)BLEMini.read();
        command ( c );
      }
  }
  
  //int16_t adc0 = ads.readADC_SingleEnded(0);
  //BLEMini.print("AIN0: ");
  //BLEMini.println(adc0);
*/
}

void serialEvent() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    command(c);
  }
}
