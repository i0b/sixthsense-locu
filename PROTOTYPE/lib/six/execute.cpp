//TODO OFF currently goes into an infinate loop
//TODO VIBRATION doesn't work / mode is not being set
#include <Arduino.h>
#include <QueueList.h>

#include "adafruit.h"
#include "actuator.h"
#include "execute.h"

#define KEEPALIVE_TIMEOUT 200 // 200 * 10 ms = 2 sec

namespace execute {

  // executeable functions
  void execute_off ( uint8_t uuid, int parameter [ 2 ] );
  void execute_vibration ( uint8_t uuid, int parameter [ 2 ] );
  void execute_servo ( uint8_t uuid, int parameter [ 2 ] );
  void execute_temperature ( uint8_t uuid, int parameter [ 2 ] );
  void electro_stimulation ( uint8_t uuid, int parameter [ 2 ] );

  const char* EXECUTION_MODE_STRING[] = { FOREACH_MODE ( GENERATE_STRING ) };

  //const uint8_t NUMBER_ACTUATORS = sizeof ( actuator::actuators ) / sizeof ( actuator_t );

  uint32_t EXECUTION_TIMER;
  uint16_t KEEPALIVE_TIMER;

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


    // initiallize heartbeat
    KEEPALIVE_TIMER = KEEPALIVE_TIMEOUT;


    // turn all outputs off
    for ( uint8_t uuid = 0; uuid < NUMBER_ACTUATORS; uuid++ ) {
      for ( int channel = 0; channel < actuator::actuators [ uuid ].number_elements; channel++ ) {
        if ( actuator::actuators [ uuid ].type != actuator::ELECTRO_CONTROLLER ) {
          adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, channel, _OFF );
        }
        else {
          adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, channel,  _ON );
        }
      }
    }

    Serial.println ( "six initialized.\r\n" );

    return 0;
  }

// -------------------------------------------------------------------------------------

  int run_executor () {

    while ( !execution_queue.isEmpty() ) {
      execution_t execution_element = execution_queue.pop();
      execution_element.function ( execution_element.uuid, execution_element.parameter );

      Serial.print ( "execution_queue.count(): " );
      Serial.println ( execution_queue.count()+1 );

      Serial.print ( "actuator_uuid: " );
      Serial.println ( execution_element.uuid );

      Serial.print ( "actuator_mode: " );
      Serial.println ( EXECUTION_MODE_STRING [ actuator::actuators [ execution_element.uuid ].mode ] );

      Serial.print ( "execution_intensity: " );
      Serial.println ( execution_element.parameter [ 0 ] );

      Serial.print ( "execution_parameter: " );
      Serial.println ( execution_element.parameter [ 1 ] );
      Serial.println ();

      // TODO REMOVE!!
      actuator::actuators [ execution_element.uuid ].changed = false;
    }

    return 0;
  }

// -------------------------------------------------------------------------------------

  int ping () {
    /*
    if ( KEEPALIVE_TIMER > 0 ) {
      KEEPALIVE_TIMER = KEEPALIVE_TIMEOUT;
    }
    else {
      return -1;
    }
    */

    KEEPALIVE_TIMER = KEEPALIVE_TIMEOUT;

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

      case execute::SET_ELECTRO :
        actuator::actuators [ uuid ].mode = execute::SET_ELECTRO;
        break;
      
    }

    actuator::actuators [ uuid ].changed = true;

    return 0;

    /*
    switch ( actuator::actuators [ uuid ].type ) {

      case ( actuator::VIBRATION_ELEMENTS ) :

        switch ( mode ) {
          case execute::VIBRATION :
            actuator::actuators [ uuid ].mode = execute::VIBRATION;
            actuator::actuators [ uuid ].changed = true;
            break;

          case execute::HEARTBEAT :
            actuator::actuators [ uuid ].mode = execute::HEARTBEAT;
            actuator::actuators [ uuid ].changed = true;
            break;

          case execute::ROTATION :
            actuator::actuators [ uuid ].mode = execute::ROTATION;
            actuator::actuators [ uuid ].changed = true;
            break;

          default :
            return -1;
        }

        break;

      case ( actuator::PELTIER_ELEMENTS ) :
        if ( mode == execute::TEMPERATURE ) {
          actuator::actuators [ uuid ].mode = execute::TEMPERATURE;
          actuator::actuators [ uuid ].changed = true;
        }
        else {
          return -1;
        }
        break;

      case ( actuator::SERVO_ELEMENTS ) :
        if ( mode == execute::SERVO ) {
          actuator::actuators [ uuid ].mode = execute::SERVO;
          actuator::actuators [ uuid ].changed = true;
        }
        else {
          return -1;
        }
        break;
    
    }

    return 0;
    */
  }

// -------------------------------------------------------------------------------------

  int set_intensity ( uint8_t uuid, int intensity ) {
    actuator::actuators [ uuid ].parameter [ 0 ] = intensity;
    actuator::actuators [ uuid ].changed = true;

    return 0;
  }

// -------------------------------------------------------------------------------------

  int set_parameter ( uint8_t uuid, int parameter ) {
    actuator::actuators [ uuid ].parameter [ 1 ] = parameter;
    actuator::actuators [ uuid ].changed = true;

    return 0;
  }

// -------------------------------------------------------------------------------------

  void execute_off ( uint8_t uuid, int parameter [ 2 ] ) {

    // TODO remove
    actuator::actuators [ uuid ].changed = false;

    switch ( actuator::actuators [ uuid ].type ) {
      case ( actuator::VIBRATION_ELEMENTS ) :
        set_intensity ( uuid, 0 );
        execute_vibration ( uuid, actuator::actuators [ uuid ].parameter );
        break;
      case ( actuator::PELTIER_ELEMENTS ) :
        set_intensity ( uuid, 0 );
        execute_temperature ( uuid, actuator::actuators [ uuid ].parameter );
        break;
      case ( actuator::SERVO_ELEMENTS ) :
        set_intensity ( uuid, 90 );
        execute_servo ( uuid, actuator::actuators [ uuid ].parameter );
        break;
    }

  }

  #define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
  #define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)
  void execute_servo ( uint8_t uuid, int parameter [ 2 ] ) {

    switch ( actuator::actuators [ uuid ].type ) {
      case ( actuator::SERVO_ELEMENTS ) :
        uint16_t pulselen = map ( actuator::actuators [ uuid ].parameter [ 0 ], 0, 180, SERVOMIN, SERVOMAX );

        for ( int channel = 0; channel < actuator::actuators [ uuid ].number_elements; channel++ ) {
          adafruit::setPWM ( actuator::actuators [ uuid ].base_address, channel, 0, pulselen );
        }

        break;
    }

  }

  void execute_vibration ( uint8_t uuid, int parameter [ 2 ] ) {

    switch ( actuator::actuators [ uuid ].type ) {
      case ( actuator::VIBRATION_ELEMENTS ) :
        for ( int channel = 0; channel < actuator::actuators [ uuid ].number_elements; channel++ ) {
          adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, channel, actuator::actuators [ uuid ].parameter [ 0 ] );
        }
        break;
    }

  }

  void execute_temperature ( uint8_t uuid, int parameter [ 2 ] ) {

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

  void electro_stimulation ( uint8_t uuid, int parameter [ 2 ] ) {
    // parameter [ 0 ]: value of stimulation
    // parameter [ 1 ]: mode
    // FOR NOW: parameter [ 0 ]: 1 = LEFT, 2 = RIGHT, 3 = UP, 4 = DOWN, 5 = MODE
    #define  LEFT 2
    #define RIGHT 1
    #define    UP 0
    #define  DOWN 5
    #define  MODE 4

    switch ( actuator::actuators [ uuid ].type ) {

      case ( actuator::ELECTRO_CONTROLLER ) :
        switch ( parameter [ 0 ] ) {
          case ( 1 ) :
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address,  LEFT, _OFF );
             delay ( 80 );
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address,  LEFT,  _ON );
            break;
          case ( 2 ) :
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, RIGHT, _OFF );
             delay ( 80 );
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, RIGHT,  _ON );
            break;
          case ( 3 ) :
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address,    UP, _OFF );
             delay ( 80 );
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address,    UP,  _ON );
            break;
          case ( 4 ) :
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address,  DOWN, _OFF );
             delay ( 80 );
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address,  DOWN,  _ON );
            break;
          case ( 5 ) :
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address,  MODE, _OFF );
             delay ( 80 );
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address,  MODE,  _ON );
            break;
        }
        break;
    }

  }

// -----------------------------------------------------------------------------------------------
// ------------------------     INTERRUPT FUNCTIONS   --------------------------------------------
// -----------------------------------------------------------------------------------------------

  void timer_isr () {
    EXECUTION_TIMER++;

    if ( --KEEPALIVE_TIMER == 0 ) {

    // TODO do disconnect pattern ;
    
    }

    // DECISSION LOGIC
    //

    for ( uint8_t uuid = 0; uuid < NUMBER_ACTUATORS; uuid++ ) {

      int* parameter = actuator::actuators [ uuid ].parameter;

      switch ( actuator::actuators [ uuid ].mode ) {

        case ( execute::OFF ) :
          if ( actuator::actuators [ uuid ].changed == true ) {
            actuator::actuators [ uuid ].changed = false;
            execution_t execution_element = { uuid, { 0 }, execute_off };
            execution_queue.push ( execution_element );
          }
          break;

        case ( execute::TEMPERATURE ) :
          if ( actuator::actuators [ uuid ].type == actuator::PELTIER_ELEMENTS 
              && actuator::actuators [ uuid ].changed == true ) {
            actuator::actuators [ uuid ].changed = false;
            execution_t execution_element = { uuid, { parameter [ 0 ] }, execute_temperature };
            execution_queue.push ( execution_element );
          }
          break;

        case ( execute::SERVO ) :
          if ( actuator::actuators [ uuid ].type == actuator::SERVO_ELEMENTS 
              && actuator::actuators [ uuid ].changed == true ) {
            actuator::actuators [ uuid ].changed = false;
            execution_t execution_element = { uuid, { parameter [ 0 ] }, execute_servo };
            execution_queue.push ( execution_element );
          }
          break;

        case ( execute::VIBRATION ) :
          if ( actuator::actuators [ uuid ].type == actuator::VIBRATION_ELEMENTS 
              && actuator::actuators [ uuid ].changed == true ) {
            actuator::actuators [ uuid ].changed = false;
            execution_t execution_element = { uuid, { parameter [ 0 ] }, execute_vibration };
            execution_queue.push ( execution_element );
          }
          break;

        #define HEARTBEAT_VIBRATOR_ON_TIME 10
        case ( execute::HEARTBEAT ) :
          if ( actuator::actuators [ uuid ].type == actuator::VIBRATION_ELEMENTS ) {
            // parameter [ 0 ]: intensity
            // parameter [ 1 ]: inter beat intervall
            if ( actuator::actuators [ uuid ].changed == true ) {
              actuator::actuators [ uuid ].changed = false;
              execution_t execution_element = { uuid, { parameter [ 0 ] }, execute_vibration };
              execution_queue.push ( execution_element );
            }

            uint32_t local_interval = EXECUTION_TIMER % ( HEARTBEAT_VIBRATOR_ON_TIME + parameter [ 1 ]);

            // first beat high
            if ( local_interval == 0 ) {
              execution_t execution_element = { uuid, { parameter [ 0 ] }, execute_vibration };
              execution_queue.push ( execution_element );
            }

            // first beat low
            // turn of after on-delay passed
            else if ( local_interval == HEARTBEAT_VIBRATOR_ON_TIME ) {
            //if ( local_interval == HEARTBEAT_VIBRATOR_ON_TIME ) {
              execution_t execution_element = { uuid, { 0 }, execute_vibration };
              execution_queue.push ( execution_element );
            }
          }
          break;

        case ( execute::ROTATION ) :
          break;

        case ( execute::SET_ELECTRO ) :
          if ( actuator::actuators [ uuid ].type == actuator::ELECTRO_CONTROLLER
              && actuator::actuators [ uuid ].changed == true ) {
            actuator::actuators [ uuid ].changed = false;
            execution_t execution_element = { uuid, { parameter [ 0 ] }, electro_stimulation };
            execution_queue.push ( execution_element );
          }
          break;

      }

    }

  }

// -----------------------------------------------------------------------------------------------

}
