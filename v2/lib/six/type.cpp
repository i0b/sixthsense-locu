#include "type.h"

namespace six {

  actuatorType_t actuatorTypes[] = {
    { actuatorTypeClass::VIBRATION,   "vibration band" },
    { actuatorTypeClass::PRESSURE,    "pressure band" },
    { actuatorTypeClass::TEMPERATURE, "temperature sleeve" },
    { actuatorTypeClass::ELECTRIC,    "electric stimulation pads" }
  };

  executionMode_t executionModes[] = {
    { executionModeClass::NONE, "none" },
    { executionModeClass::OFF, "off" },
    { executionModeClass::VIBRATION, "vibration" },
    { executionModeClass::HEARTBEAT, "heartbeat" },
    { executionModeClass::ROTATION, "rotation" },
    { executionModeClass::PRESSURE, "pressure" },
    { executionModeClass::TEMPERATURE, "temperature" },
    { executionModeClass::ELECTRO, "electric stimulation" }
  };

  instruction_t instructions[] = {
    { instructionClass::LIST, "list" },
    { instructionClass::PING_DEVICE, "ping" },
    { instructionClass::DEMONSTRATE_DISCONNECT, "disconnect demo" },
    { instructionClass::GET_MODE, "get mode" },
    { instructionClass::GET_INTENSITY, "get intensity" },
    { instructionClass::GET_PARAMETER, "get parameter" },
    { instructionClass::SET_MODE, "set mode" },
    { instructionClass::SET_INTENSITY, "set intensity" },
    { instructionClass::SET_PARAMETER, "set parameter" }
  };

}
