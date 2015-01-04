#include "actuator.h"

namespace actuator {
  
  const char* TYPE_STRING[] = { FOREACH_TYPE ( GENERATE_STRING ) };

  // PLEASE UPDATE THE NUMBER_ACTUATORS CONSTANT
  // IN ACTUATOR_H BEFORE ADDING A NEW ACTUATOR
  //

  actuator_t actuators [ NUMBER_ACTUATORS ] = {
  /*
    // #0
    {
      // description
      "vibration armband: element 1",
      // # pins
      1,
      // pins: vibration
      { 30 },
      // type
      VIBRATION_ELEMENT
    },

    // #1
    {
      // description
      "vibration armband: element 2",
      // # pins
      1,
      // pins: vibration
      { 31 },
      // type
      VIBRATION_ELEMENT
    },

    // #2
    {
      // description
      "vibration armband: element 3",
      // # pins
      1,
      // pins: vibration
      { 32 },
      // type
      VIBRATION_ELEMENT
    },

    // #3
    {
      // description
      "vibration armband: element 4",
      // # pins
      1,
      // pins: vibration
      { 33 },
      // type
      VIBRATION_ELEMENT
    },
    */

    // #0
    {
      // description
      "vibration armband",
      // # pins
      4,
      // pins: vibration
      { 30, 31, 32, 33 },
      // type
      VIBRATION_ARRAY
    },
    //TODO: add vibration elements 4-7 @ pin: 34, 35, 36, 37
    
    // #1
    {
      // description
      "peltier stripe with 4 elements",
      // # pins
      8,
      // pins: 2x - PWM, DATA, CLOCK, LATCH ENABLE
      { 11, 46, 48, 47, 10, 50, 52, 51 },
      // type
      PELTIER_SHIFT
    },

    // #2
    {
      // description
      "servo amband - element 1",
      // # pins
      1,
      // pins: servo motor 1 - 8
      { 38 },
      // type
      SERVO_ELEMENT
    },
    
    // #3
    {
      // description
      "servo amband - element 2",
      // # pins
      1,
      // pins: servo motor 1 - 8
      { 39 },
      // type
      SERVO_ELEMENT
    },
    
    // #4
    {
      // description
      "servo amband - element 3",
      // # pins
      1,
      // pins: servo motor 1 - 8
      { 40 },
      // type
      SERVO_ELEMENT
    },
    
    // #5
    {
      // description
      "servo amband - element 4",
      // # pins
      1,
      // pins: servo motor 1 - 8
      { 41 },
      // type
      SERVO_ELEMENT
    },
    
    //TODO: add servo elements 4-7 @ pin: 42, 43, 44, 45

    // #6
    {
      // description
      "electro stimulation",
      // # pins
      5,
      // pins: UP, LEFT, CENTER, RIGHT, DOWN
      { 22, 23, 24, 25, 26 },
      // type
      ELECTRO_CONTROL
    }

  };

  // calculate the total number of actuators
  // const uint8_t NUMBER_ACTUATORS = sizeof ( actuators ) / sizeof ( actuator_t );

}
