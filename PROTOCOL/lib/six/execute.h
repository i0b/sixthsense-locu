#ifndef EXECUTE_H 
#define EXECUTE_H 

#include "actuator.h"

namespace execute {

  typedef enum { OFF, VIBRATION, HEARTBEAT, ROTATION } execution_mode;

  // time in 10 * N ms
  unsigned int HEARTBEAT_ON_TIME = 2;
  
  
  typedef struct {
    actuator::actuator_t* actuator;

    void (*function) ( &unsigned int timer_value, &acutator::actuator_t actuator, &int[2] parameter );
    // first parameter:  intensity
    // second parameter: function specific parameter
    int[2] parameter;
  } function_t;

  int init_executor ();
  int set_mode ( uint8_t uuid, execute::execution_mode mode );
  int set_intensity ( uint8_t uuid, int intensity );
  int set_parameter ( uint8_t uuid, int parameter );
  int set_enabled_vibrators ( &actuator::actuator_t actuator, uint8_t enable );
}

#endif
