#ifndef EXECUTE_H 
#define EXECUTE_H 

// time in 10 * N ms
#define HEARTBEAT_VIBRATOR_ON_TIME 2

#include <stddef.h>
#include <stdint.h>

#include "actuator.h"

namespace execute {

  typedef enum { OFF, VIBRATION, HEARTBEAT, ROTATION } execution_mode;
  
  typedef struct {
    actuator::actuator_t* actuator;

    void (*function) ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
    // first parameter:  intensity
    // second parameter: function specific parameter
    int parameter[2];
  } function_t;


  extern function_t executor[];



  int init_executor ();
  int set_mode ( uint8_t uuid, execute::execution_mode mode );
  int set_intensity ( uint8_t uuid, int intensity );
  int set_parameter ( uint8_t uuid, int parameter );


  // executeable functions
  void off ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void heartbeat ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void rotate ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void vibrate ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );

  // helper function
  int set_enabled_vibrators ( actuator::actuator_t& actuator, uint8_t enable );
  void timer_isr ();

}

#endif
