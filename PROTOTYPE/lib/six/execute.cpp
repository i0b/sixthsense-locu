#include <Arduino.h>
#include <QueueList.h>

#include "adafruit.h"
#include "actuator.h"
#include "execute.h"

#define KEEPALIVE_TIMEOUT 2000 // 200 * 10 ms = 2 sec

namespace execute {

  // executeable functions
  void execute_disconnect ( uint8_t uuid, int intensity, int parameter );
  void execute_off ( uint8_t uuid, int intensity, int parameter );
  void execute_servo ( uint8_t uuid, int intensity, int parameter );
  void electro_stimulation ( uint8_t uuid, int intensity, int parameter );
  void execute_temperature ( uint8_t uuid, int intensity, int parameter );
  void execute_vibration ( uint8_t uuid, int intensity, int parameter );

  const char* EXECUTION_MODE_STRING[] = { FOREACH_MODE ( GENERATE_STRING ) };

  //const uint8_t NUMBER_ACTUATORS = sizeof ( actuator::actuators ) / sizeof ( actuator_t );

  uint32_t EXECUTION_TIMER;
  uint16_t KEEPALIVE_TIMER;

  uint8_t  CONNECTED;

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
        if ( actuator::actuators [ uuid ].type != actuator::ELECTRIC ) {
          adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, channel, _OFF );
        }
        else {
          adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, channel,  _ON );
        }
      }
    }

    CONNECTED = false;

    Serial.println ( "six initialized.\r\n" );

    return 0;
  }

// -------------------------------------------------------------------------------------

  int run_executor () {

    while ( !execution_queue.isEmpty() ) {
      execution_t execution_element = execution_queue.pop();
      execution_element.function ( execution_element.uuid, execution_element.intensity, execution_element.parameter );

      /*
      Serial.print ( "execution_queue.count(): " );
      Serial.println ( execution_queue.count()+1 );

      Serial.print ( "actuator_uuid: " );
      Serial.println ( execution_element.uuid );

      Serial.print ( "actuator_mode: " );
      Serial.println ( EXECUTION_MODE_STRING [ actuator::actuators [ execution_element.uuid ].mode ] );

      Serial.print ( "execution_intensity: " );
      Serial.println ( execution_element.intensity );

      Serial.print ( "execution_parameter: " );
      Serial.println ( execution_element.parameter );
      Serial.println ();
      */

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

    //Serial.println("DEBUG: ping() - KEEP_ALIVE");
    CONNECTED = true;
    KEEPALIVE_TIMER = KEEPALIVE_TIMEOUT;

    return 0;
  }

// -------------------------------------------------------------------------------------

  int demonstrate_disconnect () {
    // TODO which uuid?? -- not hard coded!!
    execution_t execution_element = { 0, 0, 0, execute_disconnect };
    execution_queue.push ( execution_element );

    return 0;
  }


// -------------------------------------------------------------------------------------

  int set_mode ( uint8_t uuid, execute::execution_mode mode ) {

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
  }

// -------------------------------------------------------------------------------------

  int set_intensity ( uint8_t uuid, int intensity ) {
    actuator::actuators [ uuid ].intensity = intensity;
    actuator::actuators [ uuid ].changed = true;

    return 0;
  }

// -------------------------------------------------------------------------------------

  int set_parameter ( uint8_t uuid, int parameter ) {
    actuator::actuators [ uuid ].parameter = parameter;
    actuator::actuators [ uuid ].changed = true;

    return 0;
  }

// -------------------------------------------------------------------------------------

  void execute_off ( uint8_t uuid, int intensity, int parameter ) {

    switch ( actuator::actuators [ uuid ].type ) {
      case ( actuator::VIBRATION ) :
        set_intensity ( uuid, 0 );
        execute_vibration ( uuid, actuator::actuators [ uuid ].intensity, actuator::actuators [ uuid ].parameter );
        break;
      case ( actuator::TEMPERATURE ) :
        set_intensity ( uuid, 0 );
        execute_temperature ( uuid, actuator::actuators [ uuid ].intensity, actuator::actuators [ uuid ].parameter );
        break;
      case ( actuator::PRESSURE ) :
        set_intensity ( uuid, 90 );
        execute_servo ( uuid, actuator::actuators [ uuid ].intensity, actuator::actuators [ uuid ].parameter );
        break;
    }

    actuator::actuators [ uuid ].changed = false;
  }

  #define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
  #define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)
  void execute_servo ( uint8_t uuid, int intensity, int parameter ) {

    switch ( actuator::actuators [ uuid ].type ) {
      case ( actuator::PRESSURE ) :
        uint16_t pulselen = map ( actuator::actuators [ uuid ].intensity, 0, 180, SERVOMIN, SERVOMAX );

        for ( int channel = 0; channel < actuator::actuators [ uuid ].number_elements; channel++ ) {
          adafruit::setPWM ( actuator::actuators [ uuid ].base_address, channel, 0, pulselen );
        }

        break;
    }

  }

  void execute_vibration ( uint8_t uuid, int intensity, int parameter ) {
    switch ( actuator::actuators [ uuid ].type ) {
      case ( actuator::VIBRATION ) :
        for ( int channel = 0; channel < actuator::actuators [ uuid ].number_elements; channel++ ) {
          if ( ( parameter >> channel) & 1  ) {
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, channel, intensity );
          }
          else {
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, channel, _OFF );
          }
        }
        break;
    }

  }

  void execute_disconnect( uint8_t uuid, int intensity, int parameter ) {

    switch ( actuator::actuators [ uuid ].type ) {
      case ( actuator::VIBRATION ) :
        for ( int channel = 0; channel < actuator::actuators [ uuid ].number_elements+1; channel++ ) {
          if ( channel > 0 ) {
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, channel-1, _OFF );
          }
          if ( channel != actuator::actuators [ uuid ].number_elements ) {
            adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, channel,  _ON );
          }
          // CAREFUL!! USE OF DELAY
          delay(200);
        }
        break;
    }
  
  }

  void execute_temperature ( uint8_t uuid, int intensity, int parameter ) {

    switch ( actuator::actuators [ uuid ].type ) {
      case ( actuator::TEMPERATURE ) :
        for ( uint8_t peltier = 0; peltier < ( actuator::actuators [ uuid ].number_elements / 3 ); peltier++ ) {
          // parameter [ 0 ]: DIRECTION bitmap
          // parameter [ 1 ]: ON / OFF  bitmap
          // example: only peltier element 1 should be hot - parameter = { 0x0, 0x1 }
          if ( ( actuator::actuators [ uuid ].intensity >> peltier ) & 1  ) {
            // hot 
            if ( ( actuator::actuators [ uuid ].parameter >> peltier ) & 1  ) {
                adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 0,  _ON );
                adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 1, _OFF );
                adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 2,  _ON );
            }
            // cold
            else {
                adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 0, _OFF );
                adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 1,  _ON );
                adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 2,  _ON );
            }
          }
          else {
                // element_offset: PIN1, PIN2, ENABLE
                adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 0, _OFF );
                adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 1, _OFF );
                adafruit::setPERCENT ( actuator::actuators [ uuid ].base_address, 3 * peltier + 2, _OFF );
          }
        }
        break;
    }

  }

  void electro_stimulation ( uint8_t uuid, int intensity, int parameter ) {
    // parameter [ 0 ]: value of stimulation
    // parameter [ 1 ]: mode
    // FOR NOW: parameter [ 0 ]: 1 = LEFT, 2 = RIGHT, 3 = UP, 4 = DOWN, 5 = MODE
    #define  LEFT 2
    #define RIGHT 1
    #define    UP 0
    #define  DOWN 5
    #define  MODE 4

    switch ( actuator::actuators [ uuid ].type ) {

      case ( actuator::ELECTRIC ) :
        switch ( intensity ) {
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

    if ( CONNECTED && --KEEPALIVE_TIMER == 0 ) {
    // TODO do disconnect pattern ;
    // TODO don't hardcode vibration element(s)
      CONNECTED = false;

      execution_t execution_element_left_vibration  = { 0, 0, 0, execute_disconnect };
      execution_queue.push ( execution_element_left_vibration );
      //execution_t execution_element_right_vibration = { 1, { 0 }, execute_disconnect };
      //execution_queue.push ( execution_element_right_vibration );
    }

    // DECISSION LOGIC
    //

    for ( uint8_t uuid = 0; uuid < NUMBER_ACTUATORS; uuid++ ) {

      int intensity = actuator::actuators [ uuid ].intensity;
      int parameter = actuator::actuators [ uuid ].parameter;

      switch ( actuator::actuators [ uuid ].mode ) {

        case ( execute::OFF ) :
          if ( actuator::actuators [ uuid ].changed == true ) {
            actuator::actuators [ uuid ].changed = false;
            execution_t execution_element = { uuid, 0, 0, execute_off };
            execution_queue.push ( execution_element );
          }
          break;

        case ( execute::TEMPERATURE ) :
          if ( actuator::actuators [ uuid ].type == actuator::TEMPERATURE
              && actuator::actuators [ uuid ].changed == true ) {
            actuator::actuators [ uuid ].changed = false;
            execution_t execution_element = { uuid, intensity, 0, execute_temperature };
            execution_queue.push ( execution_element );
          }
          break;

        case ( execute::SERVO ) :
          if ( actuator::actuators [ uuid ].type == actuator::PRESSURE
              && actuator::actuators [ uuid ].changed == true ) {
            actuator::actuators [ uuid ].changed = false;
            execution_t execution_element = { uuid, intensity, 0, execute_servo };
            execution_queue.push ( execution_element );
          }
          break;

        case ( execute::VIBRATION ) :
          if ( actuator::actuators [ uuid ].type == actuator::VIBRATION
              && actuator::actuators [ uuid ].changed == true ) {
            actuator::actuators [ uuid ].changed = false;
            execution_t execution_element = { uuid, intensity, parameter, execute_vibration };
            execution_queue.push ( execution_element );
          }
          break;

        #define HEARTBEAT_VIBRATOR_ON_TIME 10
        case ( execute::HEARTBEAT ) :
          if ( actuator::actuators [ uuid ].type == actuator::VIBRATION ) {
            if ( actuator::actuators [ uuid ].changed == true ) {
              actuator::actuators [ uuid ].changed = false;
              execution_t execution_element = { uuid, 0, 0, execute_vibration };
              execution_queue.push ( execution_element );
            }

            uint32_t local_interval = EXECUTION_TIMER % ( HEARTBEAT_VIBRATOR_ON_TIME + parameter );

            // first beat high
            if ( local_interval == 0 ) {
              execution_t execution_element = { uuid, intensity, 0xFF, execute_vibration };
              execution_queue.push ( execution_element );
            }

            // first beat low
            else if ( local_interval == HEARTBEAT_VIBRATOR_ON_TIME ) {
              execution_t execution_element = { uuid, 0, 0, execute_vibration };
              execution_queue.push ( execution_element );
            }
          }
          break;

        case ( execute::ROTATION ) :
          if ( actuator::actuators [ uuid ].type == actuator::VIBRATION ) {
            if ( actuator::actuators [ uuid ].changed == true ) {
              actuator::actuators [ uuid ].changed = false;
              execution_t execution_element = { uuid, 0, 0, execute_vibration };
              execution_queue.push ( execution_element );
            }

            uint32_t local_interval = EXECUTION_TIMER % parameter;

            if ( local_interval == 0 ) {
              uint8_t active_channel = ( actuator::actuators [ uuid ].attribute + 1 ) % actuator::actuators [ uuid ].number_elements;
              actuator::actuators [ uuid ].attribute = active_channel;

              uint8_t channel_bitmap = 1;

              for ( uint8_t channel = 0; channel < active_channel; channel++ ) {
                channel_bitmap <<= 1;
              }

              execution_t execution_element = { uuid, intensity, channel_bitmap, execute_vibration };
              execution_queue.push ( execution_element );
            }
          }
          break;

        case ( execute::SET_ELECTRO ) :
          if ( actuator::actuators [ uuid ].type == actuator::ELECTRIC
              && actuator::actuators [ uuid ].changed == true ) {
            actuator::actuators [ uuid ].changed = false;
            execution_t execution_element = { uuid, intensity, 0, electro_stimulation };
            execution_queue.push ( execution_element );
          }
          break;

      }

    }

  }

// -----------------------------------------------------------------------------------------------

}
