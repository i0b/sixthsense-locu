#include "actuator.h"

namespace actuator {
  
  const char* TYPE_STRING[] = { FOREACH_TYPE ( GENERATE_STRING ) };

  // PLEASE UPDATE THE NUMBER_ACTUATORS CONSTANT
  // IN ACTUATOR_H BEFORE ADDING A NEW ACTUATOR
  //

  actuator_t actuators [ NUMBER_ACTUATORS ] = {
    {
      // description
      "8-bit controllable interface: left arm vibrator",
      // # pins
      8,
      // pins: vibration motor 1 - 8
      { 30, 31, 32, 33, 34, 35, 36, 37 },
      // type
      VIBRATION_ARRAY
    },
    
    {
      // description
      "Darlington array: right arm vibrator band",
      // # pins
      8,
      // pins: vibration motor 1 - 8
      { 38, 39, 40, 41, 42, 43, 44, 45 },
      // type
      VIBRATION_ARRAY
    },

    {
      // description
      "Shift + Darlington: currently not working!",
      // # pins
      4,
      // pins: PWM, DATA, CLOCK, LATCH ENABLE
      { 11, 46, 48, 47 },
      // type
      VIBRATION_RING
    },

    {
      // description
      "H-bridge: controlling peltier on left arm's first elements",
      // # pins
      2,
      // pins: PWM, DIRECTION
      { 12, 22 },
      // type
      TEMPERATURE
    }
  };

  // calculate the total number of actuators
  // const uint8_t NUMBER_ACTUATORS = sizeof ( actuators ) / sizeof ( actuator_t );

}
