#ifndef EXECUTE_H 
#define EXECUTE_H 

// time in 10 * N ms
//#define HEARTBEAT_VIBRATOR_ON_TIME 2

#include <stddef.h>
#include <stdint.h>

#define FOREACH_MODE(EXECUTION_MODE)  \
        EXECUTION_MODE(OFF)           \
        EXECUTION_MODE(VIBRATION)     \
        EXECUTION_MODE(HEARTBEAT)     \
        EXECUTION_MODE(ROTATION)      \
        EXECUTION_MODE(SERVO)         \
        EXECUTION_MODE(TEMPERATURE)   \
        EXECUTION_MODE(SET_ELECTRO)   \ 

#ifndef GENERATOR
#define GERERATOR

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#endif

namespace execute {
  //typedef enum { OFF, VIBRATION, HEARTBEAT, ROTATION } execution_mode;
  typedef enum { FOREACH_MODE ( GENERATE_ENUM ) } execution_mode;
  
  typedef struct {
    uint8_t uuid;
    int parameter[2];
    void (*function) ( uint8_t uuid, int parameter[2] );
  } execution_t;

  extern const char* EXECUTION_MODE_STRING[];

  int init_executor ();
  int run_executor ();
  int ping ();
  int demonstrate_disconnect ();

  int set_mode ( uint8_t uuid, execute::execution_mode mode );
  int set_intensity ( uint8_t uuid, int intensity );
  int set_parameter ( uint8_t uuid, int parameter );

  // helper function
  void timer_isr ();
}

#endif
