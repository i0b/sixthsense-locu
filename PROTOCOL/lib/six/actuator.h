#ifndef ACTUATOR_H
#define ACTUATOR_H

#define MAX_VAL_PINS 8

#include <stddef.h>
#include <stdint.h>

namespace actuator {

  typedef enum { VIBRATION_ARRAY, VIBRATION_RING, ELECTRICAL, TEMPERATURE } actuator_type;

  typedef struct {
    uint8_t number_pins;
    uint8_t pins[MAX_VAL_PINS];
    actuator_type type;
  } actuator_t;


  // actuator array
  extern const actuator_t actuators[];

  // total number of actuators
  extern const uint8_t NUMBER_ACTUATORS;

}

#endif
