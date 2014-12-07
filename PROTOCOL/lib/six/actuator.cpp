#include "actuator.h"

namespace actuator {

  actuator_t actuators[] = {
    {
      // # pins
      8,
      // pins: vibration motor 1 - 8
      { 30, 31, 32, 33, 34, 35, 36, 37 },
      // type
      VIBRATION_ARRAY
    },
    
    {
      // # pins
      8,
      // pins: vibration motor 1 - 8
      { 38, 39, 40, 41, 42, 43, 44, 45 },
      // type
      VIBRATION_ARRAY
    },

    {
      // # pins
      4,
      // pins: PWM, DATA, CLOCK, LATCH ENABLE
      { 11, 46, 48, 47 },
      // type
      VIBRATION_RING
    },

    {
      // # pins
      2,
      // pins: PWM, DIRECTION
      { 12, 22 },
      // type
      TEMPERATURE
    }
  };

  // calculate the total number of actuators
  uint8_t NUMBER_ACTUATORS = sizeof ( actuators ) / sizeof ( actuator_t );

}
