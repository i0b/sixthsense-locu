#include "actuator.h"

namespace actuator {
  
  const char* TYPE_STRING[] = { FOREACH_TYPE ( GENERATE_STRING ) };

  // PLEASE UPDATE THE NUMBER_ACTUATORS CONSTANT
  // IN ACTUATOR_H BEFORE ADDING A NEW ACTUATOR
  //

  actuator_t actuators [ NUMBER_ACTUATORS ] = {
    // #0
    {
      // description
      "vibration armband",
      // # pins
      4,
      // pins: element 1 - 4
      { 40, 41, 42, 43 },
      // type
      VIBRATION_ARRAY
    },

    // #1
    {
      // description
      "peltier element 1",
      // # pins
      3,
      // pins: A, B, EN
      { 22, 23, 24 },
      // type
      PELTIER_ELEMENT
    },

    // #2
    {
      // description
      "peltier element 2",
      // # pins
      3,
      // pins: A, B, EN
      { 25, 26, 27 },
      // type
      PELTIER_ELEMENT
    },

    // #3
    {
      // description
      "peltier element 3",
      // # pins
      3,
      // pins: A, B, EN
      { 28, 29, 30 },
      // type
      PELTIER_ELEMENT
    },

    // #4
    {
      // description
      "peltier element 4",
      // # pins
      3,
      // pins: A, B, EN
      { 31, 32, 33 },
      // type
      PELTIER_ELEMENT
    },

    // #5
    {
      // description
      "servo amband - element 1",
      // # pins
      1,
      // pins: servo motor 1 - 8
      { 44 },
      // type
      SERVO_ELEMENT
    },
    
    // #6
    {
      // description
      "servo amband - element 2",
      // # pins
      1,
      // pins: servo motor 1 - 8
      { 45 },
      // type
      SERVO_ELEMENT
    },
    
    // #7
    {
      // description
      "servo amband - element 3",
      // # pins
      1,
      // pins: servo motor 1 - 8
      { 46 },
      // type
      SERVO_ELEMENT
    },
    
    // #8
    {
      // description
      "servo amband - element 4",
      // # pins
      1,
      // pins: servo motor 1 - 8
      { 47 },
      // type
      SERVO_ELEMENT
    },
    
    //TODO: add servo elements 4-7 @ pin: 42, 43, 44, 45

    // #9
    {
      // description
      "electro stimulation",
      // # pins
      5,
      // pins: UP, LEFT, CENTER, RIGHT, DOWN
      { 34, 35, 36, 37, 38 },
      // type
      ELECTRO_CONTROL
    }

  };

  // calculate the total number of actuators
  // const uint8_t NUMBER_ACTUATORS = sizeof ( actuators ) / sizeof ( actuator_t );

}
