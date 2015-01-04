#include <Arduino.h>
//TODO REMOVE
#include <Servo.h>
#include <SoftPWM.h>

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

  void set_shift_register ( uint8_t* pins, uint8_t enable );
  void toggle_pin ( uint8_t& pin, uint8_t times );

  const char* EXECUTION_MODE_STRING[] = { FOREACH_MODE ( GENERATE_STRING ) };

  //const uint8_t NUMBER_ACTUATORS = sizeof ( actuator::actuators ) / sizeof ( actuator_t );

  //TODO REMOVE HACK
  Servo servos[4];

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
    SoftPWMBegin();
    servos[0].attach ( 38 );
    servos[1].attach ( 39 );
    servos[2].attach ( 40 );
    servos[3].attach ( 41 );
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
          set_shift_register ( executor [ uuid ].actuator->pins, 0xFF );
        }
        
        executor [ uuid ].function = &vibrate; 
        
        break;

      case execute::HEARTBEAT :
        executor [ uuid ].mode = execute::HEARTBEAT;
        
        if ( executor [ uuid ].actuator->type == actuator::SHIFT ) {
          // enable all vibrators
          set_shift_register ( executor [ uuid ].actuator->pins, 0xFF );
        }
        
        executor [ uuid ].function = &heartbeat;
        
        break;

      case execute::ROTATION :
        executor [ uuid ].mode = execute::ROTATION;
        
        if ( executor [ uuid ].actuator->type == actuator::SHIFT ) {
          // only one motor vibrating at the same time
          set_shift_register ( executor [ uuid ].actuator->pins, 0x01 );
        }
        
        executor [ uuid ].function = &rotate;

        break;

      case execute::SET_ELECTRO_STIMULUS :
        //if ( executor [ uuid ].actuator->type == actuator::ELECTRICAL ) {
          executor [ uuid ].mode = execute::SET_ELECTRO_STIMULUS;
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

  void set_shift_register ( uint8_t* pins, uint8_t enable ) {
    // works with 8-bit sift register
    //
    // { PWM, DATA, CLOCK, LATCH ENABLE }

    for ( uint8_t pin = 0; pin < 4; pin++ ) {
      // set data of shift register according to current bit in enable-variable
      digitalWrite ( pins [ 1 ], enable & 1 );

      // clock shift-register
      toggle_pin ( pins [ 2 ], 1 );

      enable >>= 1;
    }
    // apply values to shift register
    toggle_pin ( pins [ 3 ], 1 );

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

    switch ( actuator.type ) {
      
      case ( actuator::SHIFT ) :
        set_shift_register ( actuator.pins, 0x00 );
        break;
      case ( actuator::SERVO_ELEMENT ) :
        for ( uint8_t servo = 0; servo < 4; servo++ ) {
          servos [ servo ].write ( 80 );
        }
        break;

      case ( actuator::PELTIER_ELEMENT ) :
        digitalWrite ( actuator.pins [ 0 ], LOW );
        digitalWrite ( actuator.pins [ 1 ], LOW );
        digitalWrite ( actuator.pins [ 2 ], LOW );
        break;

      case ( actuator::PELTIER_SHIFT ) :
        for ( uint8_t shift_register = 0; 
            shift_register < ( actuator.number_pins % 4 + 1 ); 
            shift_register++ ) {

            uint8_t offset = shift_register * 4;
            digitalWrite ( actuator.pins [ 0 + offset ], LOW );
        }
        break;

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

    switch ( actuator.type ) {
      case ( actuator::SHIFT ) :

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
        break;

      case ( actuator::VIBRATION_ARRAY ) :
        // first beat high
        if ( local_interval == 0 ) {
          for ( uint8_t pin = 0; pin < actuator.number_pins; pin++ ) {
            SoftPWMSetPercent ( actuator.pins [ pin ], parameter [ 0 ] );
          }
        }

        // first beat low
        else if ( local_interval == HEARTBEAT_VIBRATOR_ON_TIME ) {
          for ( uint8_t pin = 0; pin < actuator.number_pins; pin++ ) {
            SoftPWMSetPercent ( actuator.pins [ pin ], 0 );
          }
        }
        break;
    }
  }

  void rotate ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
    
    switch ( actuator.type ) {
      case ( actuator::SHIFT ) :
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

        break;

      case ( actuator::VIBRATION_ARRAY ) :
        if ( ( timer_value % parameter [ 1 ] ) == 0 ) {
          uint8_t active_pin = timer_value % actuator.number_pins;
          uint8_t inactive_pin = (active_pin - 1) % actuator.number_pins; 

          SoftPWMSetPercent ( actuator.pins [ inactive_pin ], 0 );
          SoftPWMSetPercent ( actuator.pins [   active_pin ], parameter [ 0 ] );
          //digitalWrite ( actuator.pins [ inactive_pin ], LOW  );
          //digitalWrite ( actuator.pins [   active_pin ], HIGH );
        }
        break;
    }
  }

  void servo ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
  // for detailed information about how to write to servos see:
  // http://www.mikrocontroller.net/articles/Modellbauservo_Ansteuerung
  
    switch ( actuator.type ) {
      case ( actuator::SERVO_ELEMENT ) :
        uint8_t num = actuator.pins [ 0 ] % 38;
        servos [ num ].write ( parameter [ 0 ] );

        // delay in ms, where 1 eq 0°, 2 eq 180°
        //uint32_t value = map ( parameter [ 0 ], 1000, 2000, 0, 180 );
        //digitalWrite ( actuator.pins [ 0 ], HIGH );
        //delayMicroseconds( value );
        //digitalWrite ( actuator.pins [ 0 ], LOW );
        break;
    }

  }

  void vibrate ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {

    switch ( actuator.type ) {
      
      case ( actuator::SHIFT ) :
        set_shift_register ( actuator.pins, parameter [ 1 ] );
        //analogWrite ( actuator.pins [ 0 ], parameter [ 0 ] );
        //SoftPWM ( actuator.pins [ 0 ], parameter [ 0 ] );
        break;

      case ( actuator::VIBRATION_ELEMENT ):
        // parameter [ 0 ]: intensity 
        //SoftPWM ( actuator.pins [ 0 ], value );

        break;
    
      case ( actuator::VIBRATION_ARRAY ) :
        byte value;
        if ( parameter [ 0 ] == 0 ) {
          value = 0;
        }
        else {
          value = 1;
        }

        for ( uint8_t pin = 0; pin < actuator.number_pins; pin++ ) {
          //digitalWrite ( actuator.pins [ pin ], value );
          SoftPWMSetPercent ( actuator.pins [ pin ], parameter [ 0 ] );
        }
        break;
    }

  }

  void keep_temperature ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
    switch ( actuator.type ) {
      case ( actuator::PELTIER_ELEMENT ) :
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

        break;
      case ( actuator::PELTIER_SHIFT ) :
        for ( uint8_t shift_register = 0; 
            shift_register < ( actuator.number_pins % 4 + 1 ); 
            shift_register++ ) {

          uint8_t offset = shift_register * 4;
          // enable PWM
          if ( parameter [ 0 ] < 0 ) {
            digitalWrite ( actuator.pins [ 0 + offset ], HIGH );
          }
          else {
            digitalWrite ( actuator.pins [ 0 + offset ], LOW );
          }

          // prepare bits for shift register
          //
          uint8_t enable_bits = parameter [ 1 ];
          uint8_t enable_shift;

          for ( uint8_t peltier = 0; peltier < 4; peltier++ ) {
            if ( enable_bits & 1  == 1 ) {
              enable_shift >>= 0;
              enable_shift >>= 1;
            }
            else {
              enable_shift >>= 1;
              enable_shift >>= 0;
            }

            enable_bits >>= 1;
          }
        }
        break;
    }
  }

  void electric_stimulation ( uint32_t& timer_value, actuator::actuator_t& actuator, int* parameter ) {
    // parameter [ 0 ]: value of stimulation
    // parameter [ 1 ]: mode
    //
    switch ( actuator.type ) {
      case ( actuator::ELECTRO_CONTROL ) :
        // pins: LEFT, RIGHT, TOP, BOTTOM, CENTER
        #define UP     0
        #define LEFT   1
        #define CENTER 2
        #define RIGHT  3
        #define DOWN   4

        toggle_pin ( actuator.pins [ RIGHT ], 1 );
        toggle_pin ( actuator.pins [ DOWN  ], 5 );
        // set max value to 5
        uint8_t value;
        value = parameter [ 0 ];

        if ( parameter [ 0 ] < 1 || parameter [ 0 ] > 5 ) {
          value = 0;
        }

        toggle_pin ( actuator.pins [ UP    ], value );
        break;

      case ( actuator::SHIFT ) :
        // every second
        uint32_t local_interval = timer_value % 10;

        if ( local_interval == 0 ) {
          digitalWrite ( actuator.pins [ 0 ], HIGH );
        }
        else {
          digitalWrite ( actuator.pins [ 0 ], LOW  );
        }

        break;
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

    if ( ( executor [ SERVO_CHECK_ACTUATOR ].actuator->type == actuator::SERVO_ELEMENT ) &&
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

    if ( ( executor [ SERVO_CHECK_ACTUATOR ].actuator->type == actuator::SERVO_ELEMENT ) &&
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
