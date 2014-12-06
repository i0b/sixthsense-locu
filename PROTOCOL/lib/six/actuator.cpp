/*
 *
 * define MAX_VAL_PINS 8
 * 
 * typedef enum { VIBRATION-RING, ELECTRICAL, TEMPERATURE } actuator_type;
 * 
 * typedef struct {
 *   unsigned int uuid;
 *   unsigned int number_pins;
 *   unsigned int pins[MAX_VAL_PINS];
 *   actuator_type type;
 * } actuator_t;
 *
 *
*/

namespace actuator {

  actuator_t actuators[] = {
    {
      // # pins
      8,
      // pins: vibration motor 1 - 8
      { 38, 39, 40, 41, 42, 43, 44, 45 },
      // type
      VIBRATION
    },
    
    {
      // # pins
      4,
      // pins: PWM, DATA, CLOCK, LATCH ENABLE
      { 11, 46, 48, 47 },
      // type
      VIBRATION-RING
    },

    {
      // # pins
      2,
      // pins: PWM, DIRECTION
      { 12, 22 },
      // type
      TEMPERATURE
    }
  }

  // calculate the total number of actuators
  NUMBER_ACTUATORS = sizeof ( acutators ) / sizeof ( actuator_t );

}
