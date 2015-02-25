#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "execute.h"

#include <stddef.h>
#include <stdint.h>

#define MAX_ELEMENTS 16

#define FOREACH_TYPE(ACTUATOR_TYPE)     \
        ACTUATOR_TYPE(VIBRATION)        \
        ACTUATOR_TYPE(PRESSURE)         \
        ACTUATOR_TYPE(TEMPERATURE)      \
        ACTUATOR_TYPE(ELECTRIC)         \

#ifndef GENERATOR
#define GERERATOR

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,


#endif

namespace six {
  class Actuator {
    typedef enum { FOREACH_TYPE ( GENERATE_ENUM ) } actuatorType;

    typedef struct {
      char* description;
      uint8_t baseAddress;
      bool active;
      uint8_t frequency;
      uint8_t numberElements;
      actuator_type type;
      execute::executionMode mode;
      bool changed;
      int intensity;
      int parameter;
      int attribute;
    } actuator_t;

    struct actuatorNode {
      actuator_t actuator;
      actuatorNode* nextActuator;
    };

    public:
        static int addActuator ( char[] description, six::Actuator::actuatorType type, uint8_t numberElements=16,
                          uint8_t baseAddress = 0x40, uint16_t frequency=1000, bool active=true,
                          six::Execute::executionMode mode=EXECUTE_OFF, bool changed=false,
                          int intensity=0, int parameter=0, int attribute=0);
        static actuator_t* getActuatorByUID ( uint8_t uid );
        static uint8_t getNumberActuators ();
        
        extern static const char* ACTUATOR_TYPE_STRING[];

    private:
      static actuatorNode* _actuatorList = NULL;
      static uint8_t _numberActuators = 0;

  }
}

#endif
