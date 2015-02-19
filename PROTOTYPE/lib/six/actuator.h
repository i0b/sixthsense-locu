#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "execute.h"

#include <stddef.h>
#include <stdint.h>

#define MAX_ELEMENTS 16
#define NUMBER_ACTUATORS 2

#define FOREACH_TYPE(ACTUATOR_TYPE)       \
        ACTUATOR_TYPE(VIBRATION_ELEMENTS) \
        ACTUATOR_TYPE(SERVO_ELEMENTS)     \
        ACTUATOR_TYPE(PELTIER_ELEMENTS)   \
        ACTUATOR_TYPE(ELECTRO_CONTROLLER) \

#ifndef GENERATOR
#define GERERATOR

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,


#endif

namespace actuator {
  typedef enum { FOREACH_TYPE ( GENERATE_ENUM ) } actuator_type;

  typedef struct {
    char* description;
    uint8_t base_address;
    uint8_t frequency;
    uint8_t number_elements;
    actuator_type type;
    execute::execution_mode mode;
    bool changed;
    // first parameter:  intensity
    // second parameter: function specific parameter
    int parameter[2];
  } actuator_t;


  extern const char* TYPE_STRING[];

  extern actuator_t actuators [ NUMBER_ACTUATORS ];

  // total number of actuators
  // extern const uint8_t NUMBER_ACTUATORS;

}

#endif
