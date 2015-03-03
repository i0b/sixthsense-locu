#include "type.h"

namespace six {
  const char* actuatorTypeString[] = { "vibration band", "pressure band", "temperature sleeve", "electric stimulation pads" };
  const char* executionModeString[] = { "none", "off", "vibration", "heartbeat", "rotation", "pressure", "temperature", "electric stimulation" };
  const char* instructionsString[] = { "list", "ping", "disconnect demo", "get mode", "get intensity", "get parameter", 
                                       "set mode", "set intensity", "set parameter" };

}
