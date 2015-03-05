#include "six.h"
#include "actuator.h"
#include "executor.h"
#include "parser.h"
#include <avr/interrupt.h>

namespace six {
  Six* _six;

  SIGNAL ( TIMER3_COMPA_vect ) {
    noInterrupts();
    _six->timerIsr();
    interrupts();
  }

  Six::Six() {
  }

  void Six::begin() {
    _six = this;
    _adafruit = new Adafruit();
    _adafruit->begin();
    _actuator = new Actuator(_adafruit);
    _executor = new Executor(_actuator, _adafruit);
    _parser = new Parser(_actuator, _executor);

    // Define Timer Interrupt / Timer1 to CTC mode
    //  finally it will call the interrupt function every 10 ms
    //
    noInterrupts();           // disable all interrupts

    TCCR3A = 0;
    TCCR3B = 0;
    TCNT3  = 0;

    OCR3A = 625;            // compare match register 16MHz/256/100Hz
    TCCR3B |=(1 << WGM32);   // CTC mode
    TCCR3B |=(1 << CS32);    // 256 prescaler 
    TIMSK3 |=(1 << OCIE3A);  // enable timer compare interrupt

    interrupts();             // enable all interrupts

    BLEMini.begin(57600);

    Serial.println("six initialized.\r\n");
  }

  void Six::timerIsr() {
    _executor->timerIsr();
  }

  int Six::addActuator(char* description, actuatorTypeClass type, uint8_t numberElements, uint8_t baseAddress) {
    uint16_t frequency = 100;
    int intensity = 0;
    int parameter = 0;
    int attribute = 0;
    bool active = true;
    bool changed = false;
    uint8_t numberUsedChannels = numberElements;
    executionModeClass mode = executionModeClass::OFF;

    switch (type) {
      case (actuatorTypeClass::VIBRATION):
        frequency = 1000;
        break;
    };

    size_t debugOutputLength = 500;
    char debugOutput[debugOutputLength];

    snprintf(debugOutput, debugOutputLength,
        "newActuator = { 'id': %d, 'baseAddress': 0x%02x, 'numberElements': %d, 'description': \"%s\" }",
        //"newActuator = { 'id': %d, 'baseAddress': 0x%02x, 'actuatorType': \"%s\", 'numberElements': %d, 'description': \"%s\" }",
        _actuator->getNumberActuators(),
        baseAddress,
        //actuatorTypes[(int)type].actuatorTypeString,
        numberElements,
        description);

    Serial.println(debugOutput);


    /*
    // change number of elements to number of channels
    switch (type) {
      case (actuatorTypeClass::TEMPERATURE):
        numberUsedChannels = 3*numberElements;
        break;
      default:
        numberUsedChannels = numberElements;
        break;
    }
    */

    return _actuator->addActuator(
        description, type, numberUsedChannels, baseAddress, frequency, active, mode, changed, intensity, parameter, attribute);
  
  }

  uint8_t Six::getNumberActuators() {
    return _actuator->getNumberActuators();
  }

  void Six::addCommandCharacter(char c) {
    _parser->append(c);  
  }
  
  int Six::parseEvaluate() {
    int returnValue = _parser->parseCommand();

    if (returnValue == 0) {
      returnValue = _parser->evaluateCommand();
    }

    _parser->reset();
    return returnValue;
  }

  int Six::parseEvaluate(char* command) {
    _parser->reset();
    // this is potentionally dangerous
    size_t commandLength = strnlen(command, REQUEST_RESPONSE_PACKET_LENGTH);

    int parseReturnValue = _parser->parseCommand(command, &commandLength);
    
    if (parseReturnValue == 0) {
      return (_parser->evaluateCommand());
    }

    else {
      return parseReturnValue;
    }
  }

  void Six::runExecutor() {
    _executor->runExecutor();
  }

}
