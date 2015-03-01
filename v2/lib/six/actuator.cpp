#include "actuator.h"
#include "type.h"

namespace six {

  Actuator::Actuator() {
    _actuatorList = NULL;
    _numberActuators = 0;
  }

  int Actuator::addActuator(char* description, actuatorType type, uint8_t numberElements,
                                   uint8_t baseAddress, uint16_t frequency, bool active,
                                   executionMode mode, bool changed,
                                   int intensity, int parameter, int attribute) {

    actuator_t* actuator = new actuator_t();

    actuator->description = description;
    actuator->baseAddress = baseAddress;
    actuator->active = active;
    actuator->frequency = frequency;
    actuator->numberElements = numberElements;
    actuator->type = type;
    actuator->mode = mode;
    actuator->changed = changed;
    actuator->intensity = intensity;
    actuator->parameter = parameter;
    actuator->attribute = attribute;

    _actuatorNode_t* node = new _actuatorNode();
    node->actuator = actuator;
    node->nextActuator = NULL;

    
    if ( _numberActuators == 0 ) {
      _actuatorList = node;
    }

    else {
      _actuatorNode_t* tail = _actuatorList;

      for ( uint8_t id=0; id<_numberActuators; id++ ) {
        tail = tail->nextActuator;
      }

      tail->nextActuator = node;
    }

    _numberActuators++;

    return 0;
  }

  uint8_t Actuator::getNumberActuators() {
    return _numberActuators;
  }

  Actuator::actuator_t* Actuator::getActuatorById(uint8_t getId) {
    if ( _numberActuators == 0 || getId > _numberActuators ) {
      return NULL;
    }

    else {
      _actuatorNode_t* node = _actuatorList;

      for ( uint8_t id=0; id<getId; id++ ) {
        node = node->nextActuator;
      }

      return node->actuator;
    }
  }
}
