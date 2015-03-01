#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "type.h"

#include <stddef.h>
#include <stdint.h>

namespace six {

  class Actuator {

    public:
      Actuator();

      typedef struct {
        char* description;
        uint8_t baseAddress;
        bool active;
        uint8_t frequency;
        uint8_t numberElements;
        actuatorType type;
        executionMode mode;
        bool changed;
        int intensity;
        int parameter;
        int attribute;
      } actuator_t;

      int addActuator(char* description, actuatorType type, uint8_t numberElements=16,
                             uint8_t baseAddress = 0x40, uint16_t frequency=1000, bool active=true,
                             executionMode mode=executionMode::OFF, bool changed=false,
                             int intensity=0, int parameter=0, int attribute=0);

      actuator_t* getActuatorById(uint8_t getId);
      uint8_t getNumberActuators();

    private:
      typedef struct _actuatorNode {
        actuator_t* actuator;
        struct _actuatorNode* nextActuator;
      } _actuatorNode_t;

      _actuatorNode_t* _actuatorList;
      uint8_t _numberActuators;

  };
}

#endif
