#include <Arduino.h>

#include "actuator.h"
#include "execute.h"

namespace execute {
  //extern const uint8_t actuator::NUMBER_ACTUATORS;

  uint32_t TIMER_VALUE;
  uint8_t  ROTATION_ACTIVE_VIBRATOR;

  //function_t executor [ actuator::NUMBER_ACTUATORS ];
  function_t executor [ 3 ];

  int init_executor () {
    for ( uint8_t uuid = 0; uuid < actuator::NUMBER_ACTUATORS; uuid++ ) {
      //executor.actuator = &(acutators [ actuator ]);
      executor [ uuid ].actuator = actuator::actuators [ uuid ];

      for ( uint8_t pin = 0; pin < actuator::actuators [ uuid ].number_pins; pin++ ) {
        pinMode ( actuator::actuators [ uuid ].pins [ pin ], OUTPUT );
        
      }

      executor [ uuid ].function = &off;
    }

    // TODO setup interrupt service to timer_isr with interval: 10 ms

    return 0;
  }

  void timer_isr () {
    TIMER_VALUE++;

    for ( uint8_t uuid = 0; uuid < actuator::NUMBER_ACTUATORS; uuid++ ) {
      executor [ uuid ].function ( TIMER_VALUE, executor [ uuid ].actuator, executor [ uuid ].parameter );
    }
  }

  int set_intensity ( uint8_t uuid, int intensity ) {
    executor [ uuid ].parameter[0] = intensity;

    return 0;
  }

  int set_parameter ( uint8_t uuid, int parameter ) {
    executor [ uuid ].parameter[1] = parameter;

    return 0;
  }

  int set_mode ( uint8_t uuid, execute::execution_mode mode ) {
    //execution_mode: { OFF, VIBRATION, HEARTBEAT, ROTATION }
    switch ( mode ) {

      case execute::OFF :
        executor [ uuid ].function = &off;
        break;

      case execute::VIBRATION :
        executor [ uuid ].function = &vibrate; 

        if ( executor [ uuid ].actuator.type == actuator::VIBRATION_RING ) {
          // enable all vibrators
          set_enabled_vibrators ( executor [ uuid ].actuator, 0xFF );
        }
        
        break;

      case execute::HEARTBEAT :
        executor [ uuid ].function = &heartbeat;
        
        if ( executor [ uuid ].actuator.type == actuator::VIBRATION_RING ) {
          // enable all vibrators
          set_enabled_vibrators ( executor [ uuid ].actuator, 0xFF );
        }
        
        break;

      case execute::ROTATION :
        executor [ uuid ].function = &rotate;
        // only one motor vibrating at the same time
        set_enabled_vibrators ( executor [ uuid ].actuator, 0x01 );

        break;
    }

    return 0;
  }

  int set_enabled_vibrators ( actuator::actuator_t& actuator, uint8_t enable ) {
    // works with 8-bit sift register in vibration-ring's, where actuator, pins are:
    // { PWM, DATA, CLOCK, LATCH ENABLE }
    
    //            { 11, 46, 48, 47 },
    if ( actuator.type != actuator::VIBRATION_RING ) {
      return -1;
    }

    for ( uint8_t pin = 0; pin < actuator.number_pins; pin++ ) {
      // set data of shift register according to current bit in enable-variable
      digitalWrite ( actuator.pins [ 1 ], enable & 1 );

      // clock shift-register
      digitalWrite ( actuator.pins [ 2 ], HIGH );
      digitalWrite ( actuator.pins [ 2 ], LOW );

      enable >>= 1;
    }

    // apply values to shift register
    digitalWrite ( actuator.pins [ 3 ], HIGH );
    digitalWrite ( actuator.pins [ 3 ], LOW );
  
    return 0;
  }


// -------------------------------------------------------------------------------------

    // TODO map matches: VIBRATION - heartbeat, rotate, ...

  void off ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
    // if temperatur:
    // analogWrite ( PWM_PIN, 0 );
    //
    // if vibration:
    // digitalWrite ( ENABLE_PIN, LOW );
    //
    // if electricity:
    // actuator_t actuator = function_list.find ( uuid );
    // set_electricity_value ( actuator, 0 );
  }


  void heartbeat ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
    // uses timer value create a heartbeat motion on all pins of the actuator
    // set HEARBEAT_START, HEARBEAT_DELAY_ON, HEARTBEAT_DELAY_OFF, HEARTBEAT_INTERVAL
    // parameter[0]: intensity
    // parameter[1]: inter beat intervall
    
    
    uint32_t local_interval = timer_value % ( HEARTBEAT_VIBRATOR_ON_TIME + parameter [ 1 ] );

    if ( actuator.type != actuator::VIBRATION_RING ) {

      // first beat high
      if ( local_interval == 0 ) {
        analogWrite ( actuator.pins [ 0 ], parameter [ 0 ] );
      }

      // first beat low
      // turn of after on-delay passed
      else if ( local_interval == HEARTBEAT_VIBRATOR_ON_TIME ) {
        analogWrite ( actuator.pins [ 0 ], 0 );
      }

     /* 
      // second beat high
      else if ( timer_value % ( HEARTBEAT_DELAY_ON + parameter [ 1 ] ) == 0 ) {
        analogWrite ( actuator.pins [ 0 ], parameter [ 0 ] );
      }

      // second beat low
      else if ( timer_value % ( HEARTBEAT_DELAY_ON + parameter [ 1 ] ) == 0 ) {
      else if ( timer_value % ....HEARBEAT_DELAY_OFF) {
        analogWrite ( actuator.pins [ 0 ], 0 );
      }
     */
    }

    else {
      //TODO yet to be implemented
      //for ( vibrator = 0; vibrator < actuator.number_pins; vibrator++ ) {
      //  analogWrite ( actuator.pin [ vibrator ], LOW );
      //}
      return;
    }
  }

  void rotate ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
    
    if ( actuator.type == actuator::VIBRATION_RING ) {
      analogWrite ( actuator.pins [ 0 ], parameter [ 0 ] );

      if ( ( timer_value % parameter [ 1 ] ) == 0 ) {
        if ( ROTATION_ACTIVE_VIBRATOR == 8 ) {
          // push one to data
          digitalWrite ( actuator.pins [ 1 ], HIGH );

          // clock
          digitalWrite ( actuator.pins [ 2 ], HIGH );
          digitalWrite ( actuator.pins [ 2 ], LOW );

          // push one to data
          digitalWrite ( actuator.pins [ 1 ], LOW );

          ROTATION_ACTIVE_VIBRATOR = 1;
        }

        else {
          // clock
          digitalWrite ( actuator.pins [ 2 ], HIGH );
          digitalWrite ( actuator.pins [ 2 ], LOW );

          ROTATION_ACTIVE_VIBRATOR++;
        }
      }

      // apply values to shift register
      digitalWrite ( actuator.pins [ 3 ], HIGH );
      digitalWrite ( actuator.pins [ 3 ], LOW );
    }

    else {
      return;
    }
    
  }

  void vibrate ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {

    if ( actuator.type == actuator::VIBRATION_RING ) {
      analogWrite ( actuator.pins [ 0 ], parameter [ 0 ] );
    }

  }

}
