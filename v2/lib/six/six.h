#ifndef SIX_H
#define SIX_H

#include <stddef.h>
#include <stdint.h>

#define REQUEST_RESPONSE_PACKET_LENGTH 1024
#define MAX_ELEMENTS 16
#define BLEMini Serial1


namespace six {
  class Six {
    public:
      Six();

      enum class actuatorType { VIBRATION, PRESSURE, TEMPERATURE, ELECTRIC };
      const char* actuatorTypeString[4] = { "vibration band", "pressure band", "temperature sleeve", "electric stimulation pads" };
      enum class executionMode { NONE, OFF, VIBRATION, HEARTBEAT, ROTATION, PRESSURE, TEMPERATURE, ELECTRO };
      const char* executionModeString[8] = { "none", "off", "vibration", "heartbeat", "rotation", "pressure", 
        "temperature", "electric stimulation" };

      enum class instructions { LIST, PING_DEVICE, DEMONSTRATE_DISCONNECT, GET_MODE, GET_INTENSITY, GET_PARAMETER, 
                           SET_MODE, SET_INTENSITY, SET_PARAMETER };

      static int parseEvaluate(char* command);
      static void runExecutor();
      static int addActuator(char* description, actuatorType type, uint8_t numberElements, uint8_t baseAddress);
      static uint8_t getNumberActuators();
      static void begin();

      static uint8_t versionMajor; 
      static uint8_t versionMinor; 

    private:
      class Actuator;
      Actuator* _actuator;
      class Executor;
      Executor* _executor;
      class Parser;
      Parser* _parser;
  };
}

#endif
