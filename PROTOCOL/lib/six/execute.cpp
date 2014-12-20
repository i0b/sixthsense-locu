#include <Arduino.h>
//TODO REMOVE
#include <Servo.h>

#include "actuator.h"
#include "execute.h"

namespace execute {

  // executeable functions
  void off ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void heartbeat ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void rotate ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void servo ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void vibrate ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void keep_temperature ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  void electric_stimulation ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter );
  int set_enabled_vibrators ( actuator::actuator_t& actuator, uint8_t enable );
  // TODO type of pin??
  void toggle_pin ( uint8_t& pin, uint8_t times );

  const char* EXECUTION_MODE_STRING[] = { FOREACH_MODE ( GENERATE_STRING ) };

  //const uint8_t NUMBER_ACTUATORS = sizeof ( actuator::actuators ) / sizeof ( actuator_t );

  //TODO REMOVE
  Servo servomotor;

  uint32_t TIMER_VALUE;
  uint8_t  ROTATION_ACTIVE_VIBRATOR;

  uint8_t SERVO_CHECK_ACTUATOR;

  function_t executor [ NUMBER_ACTUATORS ];

  int init_executor () {
    for ( uint8_t uuid = 0; uuid < NUMBER_ACTUATORS; uuid++ ) {
      executor [ uuid ].actuator = &(actuator::actuators [ uuid ]);

      Serial.print ( "Executor " );
      Serial.print ( uuid );
      Serial.print ( ": initializing..." );

      Serial.print ( "  Setting pins [ " );

      for ( int8_t pin = 0; pin < actuator::actuators [ uuid ].number_pins; pin++ ) {
        pinMode ( actuator::actuators [ uuid ].pins [ pin ], OUTPUT );

        Serial.print ( actuator::actuators [ uuid ].pins [ pin ], DEC );
        Serial.print ( " " );
      }

      Serial.println ( "]" );

      executor [ uuid ].function = &off;
    }

    // Define Timer Interrupt / Timer1 to CTC mode
    //  finally it will call the interrupt function every 10 ms
    //
    noInterrupts();           // disable all interrupts
    TCCR3A = 0;
    TCCR3B = 0;
    TCNT3  = 0;

    OCR3A = 625;              // compare match register 16MHz/236/100Hz
    TCCR3B |= (1 << WGM32);   // CTC mode
    TCCR3B |= (1 << CS32);    // 236 prescaler 
    TIMSK3 |= (1 << OCIE3A);  // enable timer compare interrupt




    // servo interrupt values
    // fuction will be called every 10 us
    
    TCCR4A = 0;
    TCCR4B = 0;
    TCNT4  = 0;

    // needed: up to 2 ms = 2000 us
    #define _10uS 20 // 16000000 / 8 / 100000  ( F_CPU / PRESCALER / Hz -- 10 us = 1/10^5 )
    OCR4A = _10uS;
    TCCR4B |= (1 << WGM42);        // CTC mode - enable the output compare interrupt
    TCCR4B |= (1 << CS41);         // set prescaler of 8
    TIMSK4 |= (1 << OCIE4A);       // enable timer compare interrupt

    interrupts();             // enable all interrupts
    
    SERVO_CHECK_ACTUATOR = 0;
    ROTATION_ACTIVE_VIBRATOR = 1;

    //TODO REMOVE
    servomotor.attach ( 13 );
  /*
    pinMode ( 12, OUTPUT );
    digitalWrite ( 12, HIGH );
  */

    Serial.println ( "six initialized.\r\n" );


    return 0;
  }

  int set_mode ( uint8_t uuid, execute::execution_mode mode ) {
    //execution_mode: { OFF, VIBRATION, HEARTBEAT, ROTATION }
    /*
    Serial.print ( "Setting mode of '" );
    Serial.print ( executor [ uuid ].actuator->description );
    Serial.print ( "' to mode '" );
    Serial.print ( EXECUTION_MODE_STRING [ mode ] );
    Serial.println ( "'\r\n" );
    */

    switch ( mode ) {

      case execute::OFF :
        executor [ uuid ].mode = execute::OFF;
        executor [ uuid ].function = &off;
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

        if ( executor [ uuid ].actuator->type == actuator::SHIFT ) {
          // enable all vibrators
          set_enabled_vibrators ( *(executor [ uuid ].actuator), 0xFF );
        }
        
        executor [ uuid ].function = &vibrate; 
        
        break;

      case execute::HEARTBEAT :
        executor [ uuid ].mode = execute::HEARTBEAT;
        
        if ( executor [ uuid ].actuator->type == actuator::SHIFT ) {
          // enable all vibrators
          set_enabled_vibrators ( *(executor [ uuid ].actuator), 0xFF );
        }
        
        executor [ uuid ].function = &heartbeat;
        
        break;

      case execute::ROTATION :
        executor [ uuid ].mode = execute::ROTATION;
        
        if ( executor [ uuid ].actuator->type == actuator::SHIFT ) {
          // only one motor vibrating at the same time
          set_enabled_vibrators ( *(executor [ uuid ].actuator), 0x01 );
        }
        
        executor [ uuid ].function = &rotate;

        break;

      case execute::SET_ELECTRICAL :
        //if ( executor [ uuid ].actuator->type == actuator::ELECTRICAL ) {
          executor [ uuid ].mode = execute::SET_ELECTRICAL;
          executor [ uuid ].function = &electric_stimulation;
        //}
        //else {
        //  executor [ uuid ].mode = execute::OFF;
        //  executor [ uuid ].function = &off;
        //}

        break;
      
    }

    return 0;
  }

  int set_enabled_vibrators ( actuator::actuator_t& actuator, uint8_t enable ) {
    // works with 8-bit sift register in vibration-ring's, where actuator, pins are:
    // { PWM, DATA, CLOCK, LATCH ENABLE }
    
    //            { 11, 46, 48, 47 },
    if ( actuator.type != actuator::SHIFT ) {
      return -1;
    }

    for ( uint8_t pin = 0; pin < actuator.number_pins; pin++ ) {
      // set data of shift register according to current bit in enable-variable
      digitalWrite ( actuator.pins [ 1 ], enable & 1 );

      // clock shift-register
      toggle_pin ( actuator.pins [ 2 ], 1 );

      enable >>= 1;
    }

    // apply values to shift register
    toggle_pin ( actuator.pins [ 3 ], 1 );
  
    return 0;
  }

  //TODO TYPE OF PIN???
  void toggle_pin ( uint8_t& pin, uint8_t times ) {
    for ( uint8_t i = 0; i < times; i++ ) {
      digitalWrite ( pin, HIGH );
      digitalWrite ( pin, LOW  );
    }
  }


// -------------------------------------------------------------------------------------

    // TODO map matches: VIBRATION - heartbeat, rotate, ...

  void off ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {

    if ( actuator.type == actuator::SHIFT ) {
      set_enabled_vibrators ( actuator, 0x00 );
    }

    else if ( actuator.type == actuator::SERVO ) {
      servomotor.write ( 80 );
    }

    else if ( actuator.type == actuator::PELTIER ) {
      digitalWrite ( actuator.pins [ 0 ], LOW );
      digitalWrite ( actuator.pins [ 1 ], LOW );
      digitalWrite ( actuator.pins [ 2 ], LOW );
    }

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

    if ( actuator.type == actuator::SHIFT ) {

      // first beat high
      if ( local_interval == 0 ) {
      //analogWrite ( actuator.pins [ 0 ], parameter [ 0 ] );
        analogWrite ( actuator.pins [ 0 ], 0 ); // parameter [ 0 ] );
      }

      // first beat low
      // turn of after on-delay passed
      else if ( local_interval == HEARTBEAT_VIBRATOR_ON_TIME ) {
        analogWrite ( actuator.pins [ 0 ], 255 );
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
    
    if ( actuator.type == actuator::SHIFT ) {
      analogWrite ( actuator.pins [ 0 ], parameter [ 0 ] );

      //TODO remove hard coded value if ( ( timer_value % parameter [ 1 ] ) == 0 ) {
      if ( ( timer_value % parameter [ 1 ] ) == 0 ) {
        if ( ROTATION_ACTIVE_VIBRATOR == 4 ) {
          // push one to data
          digitalWrite ( actuator.pins [ 1 ], HIGH );

          // clock
          toggle_pin ( actuator.pins [ 2 ], 1 );

          // push one to data
          digitalWrite ( actuator.pins [ 1 ], LOW );

          ROTATION_ACTIVE_VIBRATOR = 1;
        }

        else {
          // clock
          toggle_pin ( actuator.pins [ 2 ], 1 );

          ROTATION_ACTIVE_VIBRATOR++;
        }
      }

      // apply values to shift register
      toggle_pin ( actuator.pins [ 3 ], 1 );
    }

    else if ( actuator.type == actuator::VIBRATION_ARRAY ) {
      uint8_t active_pin = timer_value % actuator.number_pins;
      uint8_t inactive_pin = (active_pin - 1) % actuator.number_pins; 

      digitalWrite ( actuator.pins [ inactive_pin ], LOW  );
      digitalWrite ( actuator.pins [   active_pin ], HIGH );
    }
  }

  void servo ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
  // for detailed information about how to write to servos see:
  // http://www.mikrocontroller.net/articles/Modellbauservo_Ansteuerung
  
    if ( actuator.type == actuator::SERVO ) {

      // delay in ms, where 1 eq 0°, 2 eq 180°
      servomotor.write ( parameter [ 0 ] );
      //uint32_t value = map ( parameter [ 0 ], 1000, 2000, 0, 180 );
      //digitalWrite ( actuator.pins [ 0 ], HIGH );
      //delayMicroseconds( value );
      //digitalWrite ( actuator.pins [ 0 ], LOW );

    }

  }

  void vibrate ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {

    if ( actuator.type == actuator::SHIFT ) {

      set_enabled_vibrators ( actuator, parameter [ 1 ] );
      //analogWrite ( actuator.pins [ 0 ], parameter [ 0 ] );

      // -- PWM seems not to work, yet: 
      //analogWrite ( actuator.pins [ 0 ], 10 );
      //analogWrite ( 11, 0 );
    
      /*
      // /(OUTPUT ENABLE)
      digitalWrite ( actuator.pins [ 0 ], LOW );
      // CLOCK
      for ( uint8_t clock = 0; clock < 8; clock++ ) {
        // DATA
        if ( clock < parameter [ 0 ] ) {
          digitalWrite ( actuator.pins [ 1 ], HIGH );
        }
        else {
          digitalWrite ( actuator.pins [ 1 ], LOW );
        }
        digitalWrite ( actuator.pins [ 2 ], HIGH );
        digitalWrite ( actuator.pins [ 2 ], LOW );
      }
      // LATCH ENABLE
      digitalWrite ( actuator.pins [ 3 ], HIGH );
      digitalWrite ( actuator.pins [ 3 ], LOW );
      */
    }

    if ( actuator.type == actuator::VIBRATION_ARRAY ) {

      byte value;
      if ( parameter [ 0 ] == 0 ) {
        value = 0;
      }
      else {
        value = 1;
      }

      for ( uint8_t pin = 0; pin < actuator.number_pins; pin++ ) {
        digitalWrite ( actuator.pins [ pin ], value );
      }
    }

  }

  void keep_temperature ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
    if ( actuator.type == actuator::PELTIER ) {
      // pins: ENABLE, PIN1, PIN2
      switch ( parameter [ 0 ] ) {
        case 0:
          digitalWrite ( actuator.pins [ 0 ], LOW );
          digitalWrite ( actuator.pins [ 1 ], LOW );
          digitalWrite ( actuator.pins [ 2 ], LOW );
          break;
        case 1:
          digitalWrite ( actuator.pins [ 0 ], HIGH );
          digitalWrite ( actuator.pins [ 1 ], HIGH );
          digitalWrite ( actuator.pins [ 2 ], LOW  );
          break;
        case 2:
          digitalWrite ( actuator.pins [ 0 ], HIGH );
          digitalWrite ( actuator.pins [ 1 ], LOW  );
          digitalWrite ( actuator.pins [ 2 ], HIGH );
          break;
      }
    }
  }

  void electric_stimulation ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
    // parameter [ 0 ]: value of stimulation
    // parameter [ 1 ]: mode
    //
    if ( actuator.type == actuator::VIBRATION_ARRAY ) {
      uint8_t left   = actuator.pins [ 0 ];
      uint8_t right  = actuator.pins [ 1 ];
      uint8_t up     = actuator.pins [ 2 ];
      uint8_t down   = actuator.pins [ 3 ];
      uint8_t center = actuator.pins [ 4 ];

      // pins: LEFT, RIGHT, TOP, BOTTOM, CENTER
      toggle_pin ( right, 1 );
      toggle_pin (  down, 9 );
      toggle_pin (    up, parameter [ 0 ] );
    }

    if ( actuator.type == actuator::SHIFT ) {
      // every second
      uint32_t local_interval = timer_value % 10;

      if ( local_interval == 0 ) {
        digitalWrite ( actuator.pins [ 0 ], HIGH );
      }
      else {
        digitalWrite ( actuator.pins [ 0 ], LOW  );
      }

      /*
      if ( local_interval == 0 ) {
        uint8_t left   = 0b00000011;
        uint8_t up     = 0b00001100;
        uint8_t down   = 0b00110000;
        
        set_enabled_vibrators ( actuator, left );
        toggle_pin ( actuator.pins [ 0 ], 1 );

        set_enabled_vibrators ( actuator, down );
        toggle_pin ( actuator.pins [ 0 ], 9 );

        set_enabled_vibrators ( actuator, up   );
        // TODO parameter [ 0 ] instead of 1
        toggle_pin ( actuator.pins [ 0 ], parameter [ 1 ] );
      }
      */
    }
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

  void servo_isr () {
    //#define _10uS 20 // 16000000 / 8 / 100000  ( F_CPU / PRESCALER / Hz -- 10 us = 1/10^5 )
    //OCR4A = _10uS;

    // reset pin of current servo - actuator
    //
    // TODO remove trigger value
    if ( digitalRead ( 12 ) == HIGH )
      digitalWrite ( 12, LOW );
    else
      digitalWrite ( 12, HIGH );

    if ( ( executor [ SERVO_CHECK_ACTUATOR ].actuator->type == actuator::SERVO ) &&
         ( executor [ SERVO_CHECK_ACTUATOR ].mode = execute::SERVO ) ) {
      digitalWrite ( executor [ SERVO_CHECK_ACTUATOR ].actuator->pins [ 0 ], LOW );
    }
    
    // select next actuator 
    if ( SERVO_CHECK_ACTUATOR > NUMBER_ACTUATORS ) {
      SERVO_CHECK_ACTUATOR = 0;
    }
    else {
      SERVO_CHECK_ACTUATOR++;
    }

    if ( ( executor [ SERVO_CHECK_ACTUATOR ].actuator->type == actuator::SERVO ) &&
         ( executor [ SERVO_CHECK_ACTUATOR ].mode = execute::SERVO ) &&
         ( executor [ SERVO_CHECK_ACTUATOR ].parameter [ 0 ] > 0 ) ) {

      digitalWrite ( executor [ SERVO_CHECK_ACTUATOR ].actuator->pins [ 0 ], HIGH );

      // servo: 1 ms equals   0°
      //        2 ms equals 180°
      //  10 us * 100 = 1 ms - 10 us * 200 = 2 ms
      OCR4A = _10uS * map ( executor [ SERVO_CHECK_ACTUATOR ].parameter [ 0 ], 0, 180, 100, 200 );
    }
    else {
      // wait preset time until continue - here: 1 ms
      OCR4A = _10uS * 100;
    }

  }

}
