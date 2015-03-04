#ifndef SIX_H
#define SIX_H

#include "type.h"
#include "actuator.h"
#include "adafruit.h"
#include "executor.h"
#include "parser.h"
#include <stddef.h>
#include <stdint.h>

namespace six {
  class Six {
    public:
      Six();

      void addCommandCharacter(char c);
      int parseEvaluate();
      int parseEvaluate(char* command);
      void runExecutor();
      int addActuator(char* description, actuatorTypeClass type, uint8_t numberElements, uint8_t baseAddress);
      uint8_t getNumberActuators();
      void begin();
      void timerIsr();

    private:
      Actuator* _actuator;
      Adafruit* _adafruit;
      Executor* _executor;
      Parser* _parser;
  };
}

#endif
