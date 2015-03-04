#include <Arduino.h>
#include "actuator.h"
#include "adafruit.h"
#include "type.h"

namespace six {

  Actuator::Actuator(Adafruit* adafruit) {
    _adafruit = adafruit;
    _actuatorList = NULL;
    _numberActuators = 0;
  }

  int Actuator::addActuator(char* description, actuatorTypeClass type, uint8_t numberElements,
                                   uint8_t baseAddress, uint16_t frequency, bool active,
                                   executionModeClass mode, bool changed,
                                   int intensity, int parameter, int attribute) {

    actuator_t* actuator = new actuator_t();

    actuator->description = description;
    actuator->baseAddress = baseAddress;
    actuator->active = active;
    actuator->type = type;
    actuator->numberElements = numberElements;
    actuator->mode = mode;
    actuator->changed = changed;
    actuator->intensity = intensity;
    actuator->parameter = parameter;
    actuator->attribute = attribute;

    _actuatorNode_t* node = new _actuatorNode();
    node->actuator = actuator;
    node->nextActuator = NULL;

    if ( _actuatorList == NULL ) {
      _actuatorList = node;
    }

    else {
      _actuatorNode_t* tail = _actuatorList;

      while ( tail->nextActuator != NULL ) {
        tail = tail->nextActuator;
      }

      tail->nextActuator = node;
    }

    _adafruit->resetPwm(actuator->baseAddress);
    _adafruit->setPwmFrequency(actuator->baseAddress, frequency);

    switch (actuator->type) {
      case (actuatorTypeClass::ELECTRIC):
        for (int channel = 0; channel < actuator->numberElements; channel++){
          _adafruit->setPercent(actuator->baseAddress, channel, _ON);
        }
        break;
      case (actuatorTypeClass::TEMPERATURE):
        for (int channel = 0; channel < actuator->numberElements*3; channel++){
          _adafruit->setPercent(actuator->baseAddress, channel, _OFF);
        }
        break;
      default:
        for (int channel = 0; channel < actuator->numberElements; channel++){
          _adafruit->setPercent(actuator->baseAddress, channel, _OFF);
        }
        break;
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
