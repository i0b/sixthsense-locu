#include <stddef.h>
#include <stdint.h>

#define MAX_ELEMENTS 16

#ifndef _SIX_H
#define _SIX_H

namespace six {
  class Six {
    public:
      Six ();

      enum class actuatorType { VIBRATION, PRESSURE, TEMPERATURE, ELECTRIC };
      const char* actuatorTypeString[4] = { "vibration band", "pressure band", "temperature sleeve", "electric stimulation pads" };
      enum class executionMode { OFF, VIBRATION, HEARTBEAT, ROTATION, PRESSURE, TEMPERATURE, ELECTRO };

      static int parseEvaluate(char* command);
      static void runExecutor();
      static int addActuator(char* description, actuatorType type, uint8_t numberElements, uint8_t baseAddress);
      static uint8_t getNumberActuators();
      static void begin();

    private:
      class Actuator;
      Actuator* _actuator;
      class Executor;
      Executor* _executor;
  };
}

#endif
