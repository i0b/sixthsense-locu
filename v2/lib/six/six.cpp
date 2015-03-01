#include "six.h"
#include "actuator.h"
#include "executor.h"
#include "parser.h"

namespace six {
  Six::Six() {
    _actuator = new Actuator();
    _executor = new Executor(_actuator);
    _parser = new Parser(_executor);
  }

  void Six::timerIsr() {
    _executor->timerIsr();
  }

  int Six::addActuator(char* description, actuatorType type, uint8_t numberElements, uint8_t baseAddress) {
    uint16_t frequency = 100;
    int intensity = 0;
    int parameter = 0;
    int attribute = 0;
    bool active = true;
    bool changed = false;
    executionMode mode = executionMode::OFF;

    if ( type == actuatorType::VIBRATION ) {
      frequency = 1000;
    }

    return _actuator->addActuator(
        description, type, numberElements, baseAddress, frequency, active, mode, changed, intensity, parameter, attribute);
  
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
