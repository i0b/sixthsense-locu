#include <Arduino.h>

#include "adafruit.h"
#include "actuator.h"
#include "execute.h"

namespace execute {

  // executeable functions
  void execute_off ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void heartbeat ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void rotate ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void servo ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void execute_vibration ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void keep_temperature ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void electric_stimulation ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );

  const char* EXECUTION_MODE_STRING[] = { FOREACH_MODE ( GENERATE_STRING ) };

  //const uint8_t NUMBER_ACTUATORS = sizeof ( actuator::actuators ) / sizeof ( actuator_t );

  uint32_t TIMER_VALUE;

  function_t executor [ NUMBER_ACTUATORS ];

  int init_executor () {
    adafruit::begin();

    while(!Serial);
    Serial.println();

    for ( uint8_t uuid = 0; uuid < NUMBER_ACTUATORS; uuid++ ) {
      adafruit::resetPWM ( actuator::actuators [ uuid ].base_address );
      adafruit::setPWMFreq( actuator::actuators [ uuid ].base_address, actuator::actuators [ uuid ].frequency );
      executor [ uuid ].actuator = &(actuator::actuators [ uuid ]);

      Serial.print ( "executor " );
      Serial.print ( uuid );
      Serial.print ( ":\t\t#" );
      Serial.println ( actuator::actuators [ uuid ].number_elements );
      Serial.print ( "description:\t\t" );
      Serial.println ( actuator::actuators [ uuid ].description );
      Serial.println ();

      executor [ uuid ].function = &execute_off;
    }

    // Define Timer Interrupt / Timer1 to CTC mode
    //  finally it will call the interrupt function every 10 ms
    //
    noInterrupts();           // disable all interrupts
    /*
    TCCR3A = 0;
    TCCR3B = 0;
    TCNT3  = 0;

    OCR3A = 625;              // compare match register 16MHz/236/100Hz
    TCCR3B |= (1 << WGM32);   // CTC mode
    TCCR3B |= (1 << CS32);    // 236 prescaler 
    TIMSK3 |= (1 << OCIE3A);  // enable timer compare interrupt
    */

    TCCR1A = 0x80;
    TCCR1B = 0x0B;
    OCR1AH = 0x61; 
    OCR1AL = 0xA7; 

    interrupts();             // enable all interrupts

    Serial.println ( "six initialized.\r\n" );

    return 0;
  }

  int set_mode ( uint8_t uuid, execute::execution_mode mode ) {
    /*
     *   ACTUATOR_TYPE(VIBRATION_ELEMENTS) \
     *   ACTUATOR_TYPE(SERVO_ELEMENTS)     \
     *   ACTUATOR_TYPE(PELTIER_ELEMENTS)   \
     *   ACTUATOR_TYPE(ELECTRO_CONTROL)    \
     *
     *
     */

    switch ( mode ) {

      case execute::OFF :
        executor [ uuid ].mode = execute::OFF;
        executor [ uuid ].function = &execute_off;
        break;

      case execute::TEMPERATURE :
        executor [ uuid ].mode = execute::TEMPERATURE;
        executor [ uuid ].function = &keep_temperature;
        break;
      
      case execute::SERVO :
        executor [ uuid ].mode = execute::SERVO;
        executor [ uuid ].function = &servo;
        break;

      case execute::VIBRATION :
        executor [ uuid ].mode = execute::VIBRATION;
        executor [ uuid ].function = &execute_vibration; 
        adafruit::setPERCENT ( 0x40, 15, 0 );
        
        break;

      case execute::HEARTBEAT :
        executor [ uuid ].mode = execute::HEARTBEAT;
        executor [ uuid ].function = &heartbeat;
        
        break;

      case execute::ROTATION :
        executor [ uuid ].mode = execute::ROTATION;
        executor [ uuid ].function = &rotate;

        break;

      case execute::SET_ELECTRICAL :
        executor [ uuid ].mode = execute::SET_ELECTRICAL;
        executor [ uuid ].function = &electric_stimulation;

        break;
      
    }

    return 0;
  }


// -------------------------------------------------------------------------------------

    // TODO map matches: VIBRATION - heartbeat, rotate, ...

  void execute_off ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
    switch ( actuator.type ) {
      case ( actuator::VIBRATION_ELEMENTS ) :
        for ( int channel = 0; channel < actuator.number_elements; channel++ ) {
          adafruit::setPERCENT ( actuator.base_address, channel, 0 );
        }
        break;
    }
  }


  void heartbeat ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
    // uses timer value create a heartbeat motion on all element_offset of the actuator
    // set HEARBEAT_START, HEARBEAT_DELAY_ON, HEARTBEAT_DELAY_OFF, HEARTBEAT_INTERVAL
    // parameter[0]: intensity
    // parameter[1]: inter beat intervall
    
    
    uint32_t local_interval = timer_value % ( HEARTBEAT_VIBRATOR_ON_TIME + parameter [ 1 ] );

    //if ( actuator.type == actuator::SHIFT ) {

      // first beat high
      if ( local_interval == 0 ) {
      //analogWrite ( actuator.element_offset [ 0 ], parameter [ 0 ] );
        analogWrite ( actuator.element_offset [ 0 ], 0 ); // parameter [ 0 ] );
      }

      // first beat low
      // turn of after on-delay passed
      else if ( local_interval == HEARTBEAT_VIBRATOR_ON_TIME ) {
        analogWrite ( actuator.element_offset [ 0 ], 255 );
      }

     /* 
      // second beat high
      else if ( timer_value % ( HEARTBEAT_DELAY_ON + parameter [ 1 ] ) == 0 ) {
        analogWrite ( actuator.element_offset [ 0 ], parameter [ 0 ] );
      }

      // second beat low
      else if ( timer_value % ( HEARTBEAT_DELAY_ON + parameter [ 1 ] ) == 0 ) {
      else if ( timer_value % ....HEARBEAT_DELAY_OFF) {
        analogWrite ( actuator.element_offset [ 0 ], 0 );
      }
     */

    //}

    else {
      //TODO yet to be implemented
      //for ( vibrator = 0; vibrator < actuator.number_elements; vibrator++ ) {
      //  analogWrite ( actuator.pin [ vibrator ], LOW );
      //}
      return;
    }
  }

  void rotate ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
  }

  void servo ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
  }

  void execute_vibration ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
    switch ( actuator.type ) {
      case ( actuator::VIBRATION_ELEMENTS ) :
        for ( int channel = 0; channel < actuator.number_elements; channel++ ) {
          adafruit::setPERCENT ( actuator.base_address, channel, parameter [ 0 ] );
        }
        break;
    }
  }

  void keep_temperature ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
    // element_offset: ENABLE, PIN1, PIN2
    switch ( parameter [ 0 ] ) {
      case 0:
        digitalWrite ( actuator.element_offset [ 0 ], LOW );
        digitalWrite ( actuator.element_offset [ 1 ], LOW );
        digitalWrite ( actuator.element_offset [ 2 ], LOW );
        break;
      case 1:
        digitalWrite ( actuator.element_offset [ 0 ], HIGH );
        digitalWrite ( actuator.element_offset [ 1 ], HIGH );
        digitalWrite ( actuator.element_offset [ 2 ], LOW  );
        break;
      case 2:
        digitalWrite ( actuator.element_offset [ 0 ], HIGH );
        digitalWrite ( actuator.element_offset [ 1 ], LOW  );
        digitalWrite ( actuator.element_offset [ 2 ], HIGH );
        break;
    }
  }

  void electric_stimulation ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
    // parameter [ 0 ]: value of stimulation
    // parameter [ 1 ]: mode
  }

// -----------------------------------------------------------------------------------------------
// ------------------------     INTERRUPT FUNCTIONS   --------------------------------------------
// -----------------------------------------------------------------------------------------------

  void timer_isr () {
    TIMER_VALUE++;

    for ( uint8_t uuid = 0; uuid < NUMBER_ACTUATORS; uuid++ ) {
      executor [ uuid ].function ( TIMER_VALUE, *executor [ uuid ].actuator, executor [ uuid ].parameter );
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

// -----------------------------------------------------------------------------------------------

}
