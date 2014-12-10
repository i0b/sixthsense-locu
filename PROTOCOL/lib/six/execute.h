#ifndef EXECUTE_H 
#define EXECUTE_H 

// time in 10 * N ms
#define HEARTBEAT_VIBRATOR_ON_TIME 2

#include <stddef.h>
#include <stdint.h>
#include "actuator.h"

#define FOREACH_MODE(EXECUTION_MODE)  \
        EXECUTION_MODE(OFF)           \
        EXECUTION_MODE(VIBRATION)     \
        EXECUTION_MODE(HEARTBEAT)     \
        EXECUTION_MODE(ROTATION)      \
        EXECUTION_MODE(WAVING)        \

#ifndef GENERATOR
#define GERERATOR

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#endif

namespace execute {
  //typedef enum { OFF, VIBRATION, HEARTBEAT, ROTATION } execution_mode;
  typedef enum { FOREACH_MODE ( GENERATE_ENUM ) } execution_mode;
  
  typedef struct {
    actuator::actuator_t* actuator;

    execution_mode mode;

    void (*function) ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
    // first parameter:  intensity
    // second parameter: function specific parameter
    int parameter[2];
  } function_t;


  extern function_t executor[];
  
  extern const char* EXECUTION_MODE_STRING[];


  int init_executor ();
  int set_mode ( uint8_t uuid, execute::execution_mode mode );
  int set_intensity ( uint8_t uuid, int intensity );
  int set_parameter ( uint8_t uuid, int parameter );


  // helper function
  void timer_isr ();

}

#endif
