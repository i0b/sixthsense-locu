#ifndef ADAFRUIT_H
#define ADAFRUIT_H

#include <stddef.h>
#include <stdint.h>

#define  _ON ( 100 )
#define _OFF (   0 )
//#define PCA9685_SUBADR1 0x2
//#define PCA9685_SUBADR2 0x3
//#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

namespace six {

  class Adafruit {

    public:
      void begin(void);
      int checkActuatorActive(uint8_t i2cAddress);
      void setPercent(uint8_t i2cAddress, uint8_t channel, uint8_t percent);
      void setPwm(uint8_t i2cAddress, uint8_t num, uint16_t on, uint16_t off);
      void setPwmFrequency(uint8_t i2cAddress, float frequency);
      void resetPwm(uint8_t i2cAddress);

    private:
      uint8_t _read8(uint8_t i2cAddress, uint8_t channel);
      void _write8(uint8_t i2cAddress, uint8_t channel, uint8_t d);

  };

}

#endif
