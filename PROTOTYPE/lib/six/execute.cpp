#include <Arduino.h>
#include <QueueList.h>

#include "adafruit.h"
#include "actuator.h"
#include "execute.h"

namespace execute {

  // executeable functions
  void execute_off ( uint8_t uuid );
  void execute_vibration ( uint8_t uuid );
  void execute_servo ( uint8_t uuid );
  void execute_temperature ( uint8_t uuid );

  void heartbeat ( uint8_t uuid );
  void rotate ( uint8_t uuid );
  void electric_stimulation ( uint8_t uuid );

  const char* EXECUTION_MODE_STRING[] = { FOREACH_MODE ( GENERATE_STRING ) };

  //const uint8_t NUMBER_ACTUATORS = sizeof ( actuator::actuators ) / sizeof ( actuator_t );

  uint32_t TIMER_VALUE;
  QueueList <execution_t> execution_queue;

// -------------------------------------------------------------------------------------

  int init_executor () {
    adafruit::begin();

    while(!Serial);
    Serial.println();

    for ( uint8_t uuid = 0; uuid < NUMBER_ACTUATORS; uuid++ ) {
      adafruit::resetPWM ( actuator::actuators [ uuid ].base_address );
      adafruit::setPWMFreq( actuator::actuators [ uuid ].base_address, actuator::actuators [ uuid ].frequency );

      Serial.print ( "executor " );
      Serial.print ( uuid );
      Serial.print ( ":\t\t#" );
      Serial.println ( actuator::actuators [ uuid ].number_elements );
      Serial.print ( "description:\t\t" );
      Serial.println ( actuator::actuators [ uuid ].description );
      Serial.println ();
    }

    // Define Timer Interrupt / Timer1 to CTC mode
    //  finally it will call the interrupt function every 10 ms
    //
    noInterrupts();           // disable all interrupts

    TCCR3A = 0;
    TCCR3B = 0;
    TCNT3  = 0;

    OCR3A = 625;            // compare match register 16MHz/256/100Hz
    TCCR3B |= (1 << WGM32);   // CTC mode
    TCCR3B |= (1 << CS32);    // 256 prescaler 
    TIMSK3 |= (1 << OCIE3A);  // enable timer compare interrupt
/*
    TCCR1A = 0x80;
    TCCR1B = 0x0B;
    OCR1AH = 0x61; 
    OCR1AL = 0xA7; 
*/

    interrupts();             // enable all interrupts

    Serial.println ( "six initialized.\r\n" );

    return 0;
  }

// -------------------------------------------------------------------------------------

  int run_executor () {

    while ( !execution_queue.isEmpty() ) {
      execution_t execution_element = execution_queue.pop();
      execution_element.function ( execution_element.uuid );
    }

    return 0;
  }

// -------------------------------------------------------------------------------------

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
        actuator::actuators [ uuid ].mode = execute::OFF;
        break;

      case execute::TEMPERATURE :
        actuator::actuators [ uuid ].mode = execute::TEMPERATURE;
        break;
      
      case execute::SERVO :
        actuator::actuators [ uuid ].mode = execute::SERVO;
        break;

      case execute::VIBRATION :
        actuator::actuators [ uuid ].mode = execute::VIBRATION;
        break;

      case execute::HEARTBEAT :
        actuator::actuators [ uuid ].mode = execute::HEARTBEAT;
        break;

      case execute::ROTATION :
        actuator::actuators [ uuid ].mode = execute::ROTATION;
        break;

      case execute::SET_ELECTRICAL :
        actuator::actuators [ uuid ].mode = execute::SET_ELECTRICAL;
        break;
      
    }

    return 0;
  }

// -------------------------------------------------------------------------------------

  int set_intensity ( uint8_t uuid, int intensity ) {
    actuator::actuators [ uuid ].parameter[0] = intensity;
    actuator::actuators [ uuid ].changed = true;

    return 0;
  }

// -------------------------------------------------------------------------------------

  int set_parameter ( uint8_t uuid, int parameter ) {
    actuator::actuators [ uuid ].parameter[1] = parameter;
    actuator::actuators [ uuid ].changed = true;

    return 0;
  }

// -------------------------------------------------------------------------------------

    // TODO map matches: VIBRATION - heartbeat, rotate, ...

  void execute_off ( uint8_t uuid ) {

    switch ( actuator::actuators [ uuid ].type ) {
      case ( actuator::VIBRATION_ELEMENTS ) :
        set_intensity ( uuid, 0 );
        execute_vibration ( uuid );
        break;
      case ( actuator::PELTIER_ELEMENTS ) :
        set_intensity ( uuid, 0 );
        execute_temperature ( uuid );
        break;
      case ( actuator::SERVO_ELEMENTS ) :
        set_intensity ( uuid, 90 );
        execute_servo ( uuid );
        break;
    }

  }


  void heartbeat ( uint8_t uuid ) {
    // uses timer value create a heartbeat motion on all element_offset of the actuator
    // set HEARBEAT_START, HEARBEAT_DELAY_ON, HEARTBEAT_DELAY_OFF, HEARTBEAT_INTERVAL
    // parameter[0]: intensity
    // parameter[1]: inter beat intervall
    
    
    /*uint32_t local_interval = timer_value % ( HEARTBEAT_VIBRATOR_ON_TIME + parameter [ 1 ] );

    //if ( actuator.type == actuator::SHIFT ) {

      // first beat high
      if ( local_interval == 0 ) {
      //analogWrite ( actuator.element_offset [ 0 ], parameter [ 0 ] );
      }

      // first beat low
      // turn of after on-delay passed
      else if ( local_interval == HEARTBEAT_VIBRATOR_ON_TIME ) {
      //analogWrite ( actuator.element_offset [ 0 ], 255 );
      }
      */

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

    /*
    else {
      //TODO yet to be implemented
      //for ( vibrator = 0; vibrator < actuator.number_elements; vibrator++ ) {
      //  analogWrite ( actuator.pin [ vibrator ], LOW );
      //}
      return;
    }
    */
  }

  void rotate ( uint8_t uuid ) {
  }

  #define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
  #define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)
  void execute_servo ( uint8_t uuid ) {

    switch ( actuator::actuators [ uuid ].type ) {
            case ( actuator::VIBRATION_ELEMENTS ) :
              uint16_t pulselen = map ( actuator::actuators [ uuid ].parameter [ 0 ], 0, 180, SERVOMIN, SERVOMAX );

              for ( int channel = 0; channel < actuator::actuators [ uuid ].number_elements; channel++ ) {
                adafruit::setPWM ( actuator::actuators [ uuid ].base_address, channel, 0, pulselen );
              }
              break;
    }

  }

  void execute_vibration ( uint8_t uuid ) {
 
    switch ( actuator::actuators [ uuid ].type ) {
      case ( actuator::VIBRATION_ELEMENTS ) :
        for ( int channel = 0; channel < actuator::actuators [ uuid ].number_elements; channel++ ) {
          adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, channel, actuator::actuators [ uuid ].parameter [ 0 ] );
        }
        break;
    }

  }

  void execute_temperature ( uint8_t uuid ) {

    // element_offset: PIN1, PIN2, ENABLE
    switch ( actuator::actuators [ uuid ].type ) {
      case ( actuator::PELTIER_ELEMENTS ) :
        switch ( actuator::actuators [ uuid ].parameter [ 0 ] ) {
          //case 0:
          case 1:
            for ( uint8_t peltier = 0; peltier < ( actuator::actuators [ uuid ].number_elements / 3 ); peltier++ ) {
              adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 0, _OFF );
              adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 1,  _ON );
              adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 2,  _ON );
            }
            break;
          case 2:
            for ( uint8_t peltier = 0; peltier < ( actuator::actuators [ uuid ].number_elements / 3 ); peltier++ ) {
              adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 0,  _ON );
              adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 1, _OFF );
              adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 2,  _ON );
            }
            break;
          default:
            for ( uint8_t peltier = 0; peltier < ( actuator::actuators [ uuid ].number_elements / 3 ); peltier++ ) {
              adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 0, _OFF );
              adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 1, _OFF );
              adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 2, _OFF );
            }
            break;
        }
        break;
    }

  }

  void electric_stimulation ( uint8_t uuid ) {
    // parameter [ 0 ]: value of stimulation
    // parameter [ 1 ]: mode
  }

// -----------------------------------------------------------------------------------------------
// ------------------------     INTERRUPT FUNCTIONS   --------------------------------------------
// -----------------------------------------------------------------------------------------------

  void timer_isr () {
    TIMER_VALUE++;

    // DECISSION LOGIC
    //

    for ( uint8_t uuid = 0; uuid < NUMBER_ACTUATORS; uuid++ ) {

      switch ( actuator::actuators [ uuid ].type ) {
        case ( actuator::VIBRATION_ELEMENTS ) :
          if ( actuator::actuators [ uuid ].changed == true ) {
            execution_t execution_element = { uuid, execute_vibration };
            execution_queue.push ( execution_element );
            actuator::actuators [ uuid ].changed = false;
          }
          break;
        case ( actuator::PELTIER_ELEMENTS ) :
          if ( actuator::actuators [ uuid ].changed == true ) {
            execution_t execution_element = { uuid, execute_temperature };
            execution_queue.push ( execution_element );
            actuator::actuators [ uuid ].changed = false;
          }
          break;
        case ( actuator::SERVO_ELEMENTS ) :
          if ( actuator::actuators [ uuid ].changed == true ) {
            execution_t execution_element = { uuid, execute_servo };
            execution_queue.push ( execution_element );
            actuator::actuators [ uuid ].changed = false;
          }
          break;
      }
    }

  }

// -----------------------------------------------------------------------------------------------

}
