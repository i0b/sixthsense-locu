#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stddef.h>
#include <stdint.h>


#define MAX_VAL_PINS 8
#define NUMBER_ACTUATORS 4

#define FOREACH_TYPE(ACTUATOR_TYPE)    \
        ACTUATOR_TYPE(VIBRATION_ARRAY) \
        ACTUATOR_TYPE(VIBRATION_RING)  \
        ACTUATOR_TYPE(ELECTRICAL)      \
        ACTUATOR_TYPE(TEMPERATURE)     \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,


namespace actuator {
  typedef enum { FOREACH_TYPE ( GENERATE_ENUM ) } actuator_type;
  //typedef enum { VIBRATION_ARRAY, VIBRATION_RING, ELECTRICAL, TEMPERATURE } actuator_type;

  typedef struct {
    char* description;
    uint8_t number_pins;
    uint8_t pins [ MAX_VAL_PINS ];
    actuator_type type;
  } actuator_t;


  extern const char* TYPE_STRING[];

  extern actuator_t actuators [ NUMBER_ACTUATORS ];

  // total number of actuators
  // extern const uint8_t NUMBER_ACTUATORS;

}

#endif
