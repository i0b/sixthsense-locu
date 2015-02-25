#include "adafruit.h"
#include <Wire.h>

namespace adafruit {

  uint8_t   conversionDelay;
  uint8_t   bitShift;
  adsGain_t gain;


  void begin() {
    Wire.begin();

    conversionDelay = ADS1115_CONVERSIONDELAY;
    bitShift = 0;
    gain = GAIN_TWOTHIRDS; // +/- 6.144V range (limited to VDD +0.3V max!)
  }

  //---------------------------------------------------------------------------------------------------------------------------
  //  PWM device
  //---------------------------------------------------------------------------------------------------------------------------

  void resetPWM(uint8_t i2c_address) {
   write8(i2c_address, PCA9685_MODE1, 0x0);
  }

  void setPWMFreq(uint8_t i2c_address, float freq) {
    float prescaleval = 25000000;
    prescaleval /= 4096;
    prescaleval /= freq;
    prescaleval -= 1;
    //Serial.print("Estimated pre-scale: "); Serial.println(prescaleval);
    uint8_t prescale = floor(prescaleval + 0.5);
    //Serial.print("Final pre-scale: "); Serial.println(prescale);  
    
    uint8_t oldmode = read8(i2c_address, PCA9685_MODE1);
    uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
    write8(i2c_address, PCA9685_MODE1, newmode); // go to sleep
    write8(i2c_address, PCA9685_PRESCALE, prescale); // set the prescaler
    write8(i2c_address, PCA9685_MODE1, oldmode);
    delay(5);
    write8(i2c_address, PCA9685_MODE1, oldmode | 0xa1);  //  This sets the MODE1 register to turn on auto increment.
                                                         // This is why the beginTransmission below was not working.
    //  Serial.print("Mode now 0x"); Serial.println(read8(PCA9685_MODE1), HEX);
  }

  void setPWM(uint8_t i2c_address, uint8_t num, uint16_t on, uint16_t off) {
    Wire.beginTransmission(i2c_address);
    Wire.write(LED0_ON_L+4*num);
    Wire.write(on);
    Wire.write(on>>8);
    Wire.write(off);
    Wire.write(off>>8);
    Wire.endTransmission();
  }

  void setPERCENT(uint8_t i2c_address, uint8_t channel, uint8_t percent) {
    // Low value equals high ratio
    //uint16_t val = map ( percent, 0, 100, 4095, 0 );
    uint16_t val = map ( percent, 100, 0, 4095, 0 );

    if (val == 0) {
      // Special value for signal fully on.
      setPWM ( i2c_address, channel, 0, 4096 );
    }
    else if (val == 4095) {
      // Special value for signal fully off.
      setPWM ( i2c_address, channel, 4096, 0 );
    }
    else {
      setPWM ( i2c_address, channel, 0, val);
    }

  }

  uint8_t read8(uint8_t i2c_address, uint8_t addr) {
    Wire.beginTransmission(i2c_address);
    Wire.write(addr);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t)i2c_address, (uint8_t)1);
    return Wire.read();
  }

  void write8(uint8_t i2c_address, uint8_t addr, uint8_t d) {
    Wire.beginTransmission(i2c_address);
    Wire.write(addr);
    Wire.write(d);
    Wire.endTransmission();
  }



  //---------------------------------------------------------------------------------------------------------------------------
  //  ADC device
  //---------------------------------------------------------------------------------------------------------------------------


  /**************************************************************************/
  /*!
      @brief  Abstract away platform differences in Arduino wire library
  */
  /**************************************************************************/
  static uint8_t i2cread(void) {
    #if ARDUINO >= 100
    return Wire.read();
    #else
    return Wire.receive();
    #endif
  }


  /**************************************************************************/
  /*!
      @brief  Abstract away platform differences in Arduino wire library
  */
  /**************************************************************************/
  static void i2cwrite(uint8_t x) {
    #if ARDUINO >= 100
    Wire.write((uint8_t)x);
    #else
    Wire.send(x);
    #endif
  }


  /**************************************************************************/
  /*!
      @brief  Writes 16-bits to the specified destination register
  */
  /**************************************************************************/
  static void writeRegister(uint8_t i2c_address, uint8_t reg, uint16_t value) {
    Wire.beginTransmission(i2c_address);
    i2cwrite((uint8_t)reg);
    i2cwrite((uint8_t)(value>>8));
    i2cwrite((uint8_t)(value & 0xFF));
    Wire.endTransmission();
  }


  /**************************************************************************/
  /*!
      @brief  Writes 16-bits to the specified destination register
  */
  /**************************************************************************/
  static uint16_t readRegister(uint8_t i2c_address, uint8_t reg) {
    Wire.beginTransmission(i2c_address);
    i2cwrite(ADS1015_REG_POINTER_CONVERT);
    Wire.endTransmission();
    Wire.requestFrom(i2c_address, (uint8_t)2);
    return ((i2cread() << 8) | i2cread());  
  }

  /**************************************************************************/
  /*!
      @brief  Gets a single-ended ADC reading from the specified channel
  */
  /**************************************************************************/
  uint16_t readADC_SingleEnded(uint8_t i2c_address, uint8_t channel) {
    if (channel > 3)
    {
      return 0;
    }
    
    // Start with default values
    uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                      ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    // Set PGA/voltage range
    config |= gain;

    // Set single-ended input channel
    switch (channel)
    {
      case (0):
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
        break;
      case (1):
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
        break;
      case (2):
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
        break;
      case (3):
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
        break;
    }

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;

    // Write config register to the ADC
    writeRegister(i2c_address, ADS1015_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    delay(conversionDelay);

    // Read the conversion results
    // Shift 12-bit results right 4 bits for the ADS1015
    return readRegister(i2c_address, ADS1015_REG_POINTER_CONVERT) >> bitShift;  
  }


  /**************************************************************************/
  /*! 
      @brief  Reads the conversion results, measuring the voltage
              difference between the P (AIN0) and N (AIN1) input.  Generates
              a signed value since the difference can be either
              positive or negative.
  */
  /**************************************************************************/
  int16_t readADC_Differential_0_1(uint8_t i2c_address) {
    // Start with default values
    uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                      ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    // Set PGA/voltage range
    config |= gain;
                      
    // Set channels
    config |= ADS1015_REG_CONFIG_MUX_DIFF_0_1;          // AIN0 = P, AIN1 = N

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;

    // Write config register to the ADC
    writeRegister(i2c_address, ADS1015_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    delay(conversionDelay);

    // Read the conversion results
    uint16_t res = readRegister(i2c_address, ADS1015_REG_POINTER_CONVERT) >> bitShift;
    if (bitShift == 0)
    {
      return (int16_t)res;
    }
    else
    {
      // Shift 12-bit results right 4 bits for the ADS1015,
      // making sure we keep the sign bit intact
      if (res > 0x07FF)
      {
        // negative number - extend the sign to 16th bit
        res |= 0xF000;
      }
      return (int16_t)res;
    }
  }


  /**************************************************************************/
  /*! 
      @brief  Reads the conversion results, measuring the voltage
              difference between the P (AIN2) and N (AIN3) input.  Generates
              a signed value since the difference can be either
              positive or negative.
  */
  /**************************************************************************/
  int16_t readADC_Differential_2_3(uint8_t i2c_address) {
    // Start with default values
    uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                      ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    // Set PGA/voltage range
    config |= gain;

    // Set channels
    config |= ADS1015_REG_CONFIG_MUX_DIFF_2_3;          // AIN2 = P, AIN3 = N

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;

    // Write config register to the ADC
    writeRegister(i2c_address, ADS1015_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    delay(conversionDelay);

    // Read the conversion results
    uint16_t res = readRegister(i2c_address, ADS1015_REG_POINTER_CONVERT) >> bitShift;
    if (bitShift == 0)
    {
      return (int16_t)res;
    }
    else
    {
      // Shift 12-bit results right 4 bits for the ADS1015,
      // making sure we keep the sign bit intact
      if (res > 0x07FF)
      {
        // negative number - extend the sign to 16th bit
        res |= 0xF000;
      }
      return (int16_t)res;
    }
  }


  /**************************************************************************/
  /*!
      @brief  Sets up the comparator to operate in basic mode, causing the
              ALERT/RDY pin to assert (go from high to low) when the ADC
              value exceeds the specified threshold.

              This will also set the ADC in continuous conversion mode.
  */
  /**************************************************************************/
  void startComparator_SingleEnded(uint8_t i2c_address, uint8_t channel, int16_t threshold)
  {
    // Start with default values
    uint16_t config = ADS1015_REG_CONFIG_CQUE_1CONV   | // Comparator enabled and asserts on 1 match
                      ADS1015_REG_CONFIG_CLAT_LATCH   | // Latching mode
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                      ADS1015_REG_CONFIG_MODE_CONTIN  | // Continuous conversion mode
                      ADS1015_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode

    // Set PGA/voltage range
    config |= gain;
                      
    // Set single-ended input channel
    switch (channel)
    {
      case (0):
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
        break;
      case (1):
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
        break;
      case (2):
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
        break;
      case (3):
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
        break;
    }

    // Set the high threshold register
    // Shift 12-bit results left 4 bits for the ADS1015
    writeRegister(i2c_address, ADS1015_REG_POINTER_HITHRESH, threshold << bitShift);

    // Write config register to the ADC
    writeRegister(i2c_address, ADS1015_REG_POINTER_CONFIG, config);
  }

  /**************************************************************************/
  /*!
      @brief  In order to clear the comparator, we need to read the
              conversion results.  This function reads the last conversion
              results without changing the config value.
  */
  /**************************************************************************/
  int16_t getLastConversionResults(uint8_t i2c_address)
  {
    // Wait for the conversion to complete
    delay(adafruit::conversionDelay);

    // Read the conversion results
    uint16_t res = readRegister(i2c_address, ADS1015_REG_POINTER_CONVERT) >> adafruit::bitShift;
    if (adafruit::bitShift == 0)
    {
      return (int16_t)res;
    }
    else
    {
      // Shift 12-bit results right 4 bits for the ADS1015,
      // making sure we keep the sign bit intact
      if (res > 0x07FF)
      {
        // negative number - extend the sign to 16th bit
        res |= 0xF000;
      }
      return (int16_t)res;
    }
  }

}
