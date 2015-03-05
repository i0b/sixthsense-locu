#include "adafruit.h"
#include <Wire.h>
#include <Arduino.h>

namespace six {

  void Adafruit::begin() {
    Wire.begin();
  }

  int Adafruit::checkActuatorActive(uint8_t i2cAddress) {
    /*
    uint8_t mode = _read8(i2cAddress, PCA9685_MODE1); 
    char buffer[100];
    snprintf(buffer, sizeof buffer, "DEBUG: read8(%x): %x", i2cAddress, mode);
    Serial.println(buffer);
    */

    if (_read8(i2cAddress, PCA9685_MODE1) != 0xFF) {
      return 0;
    }
    else {
      return -1;
    }
  }

  void Adafruit::resetPwm(uint8_t i2cAddress) {
   _write8(i2cAddress, PCA9685_MODE1, 0x0);
  }

  void Adafruit::setPwmFrequency(uint8_t i2cAddress, float frequency) {
    float prescaleval = 25000000;
    prescaleval /= 4096;
    prescaleval /= frequency;
    prescaleval -= 1;
    uint8_t prescale = floor(prescaleval + 0.5);
    
    uint8_t oldmode = _read8(i2cAddress, PCA9685_MODE1);
    uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
    _write8(i2cAddress, PCA9685_MODE1, newmode); // go to sleep
    _write8(i2cAddress, PCA9685_PRESCALE, prescale); // set the prescaler
    _write8(i2cAddress, PCA9685_MODE1, oldmode);
    delay(5);
    _write8(i2cAddress, PCA9685_MODE1, oldmode | 0xa1);  // This sets the MODE1 register to turn on auto increment.
                                                          // This is why the beginTransmission below was not working.
    // Serial.print("Mode now 0x"); Serial.println(read8(PCA9685_MODE1), HEX);
  }

  void Adafruit::setPwm(uint8_t i2cAddress, uint8_t num, uint16_t on, uint16_t off) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(LED0_ON_L+4*num);
    Wire.write(on);
    Wire.write(on>>8);
    Wire.write(off);
    Wire.write(off>>8);
    Wire.endTransmission();
  }

  void Adafruit::setPercent(uint8_t i2cAddress, uint8_t channel, uint8_t percent) {
    // Low value equals high ratio
    //uint16_t val = map ( percent, 0, 100, 4095, 0 );
    uint16_t val = map(percent, 100, 0, 4095, 0);

    if (val == 0) {
      // Special value for signal fully on.
      setPwm(i2cAddress, channel, 0, 4096);
    }
    else if (val == 4095) {
      // Special value for signal fully off.
      setPwm(i2cAddress, channel, 4096, 0);
    }
    else {
      setPwm(i2cAddress, channel, 0, val);
    }

  }

  uint8_t Adafruit::_read8(uint8_t i2cAddress, uint8_t channel) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(channel);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t)i2cAddress, (uint8_t)1);
    return Wire.read();
  }

  void Adafruit::_write8(uint8_t i2cAddress, uint8_t channel, uint8_t d) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(channel);
    Wire.write(d);
    Wire.endTransmission();
  }

}
