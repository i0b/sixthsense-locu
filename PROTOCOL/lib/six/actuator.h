#ifndef ACTUATOR_H
#define ACTUATOR_H

#define MAX_VAL_PINS 8

namespace actuator {

  typedef enum { VIBRATION, VIBRATION-RING, ELECTRICAL, TEMPERATURE } actuator_type;

  typedef struct {
    uint8_t number_pins;
    uint8_t pins[MAX_VAL_PINS];
    actuator_type type;
  } actuator_t;

  uint8_t NUMBER_ACTUATORS

}

#endif
