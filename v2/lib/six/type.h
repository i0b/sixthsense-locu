#ifndef TYPE_H
#define TYPE_H

#define REQUEST_RESPONSE_PACKET_LENGTH 1024
#define MAX_ELEMENTS 16
#define BLEMini Serial1

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

namespace six {

  enum class actuatorTypeClass { VIBRATION, PRESSURE, TEMPERATURE, ELECTRIC };

  typedef struct {
  actuatorTypeClass actuatorTypeEnum;
  char* actuatorTypeString;
  } actuatorType_t;

  extern actuatorType_t actuatorTypes[];


  enum class executionModeClass { NONE, OFF, VIBRATION, HEARTBEAT, ROTATION, PRESSURE, TEMPERATURE, ELECTRO };

  typedef struct {
  executionModeClass executionModeEnum;
  char* executionModeString;
  } executionMode_t;

  extern executionMode_t executionModes[];


  enum class instructionClass { LIST, PING_DEVICE, DEMONSTRATE_DISCONNECT, GET_MODE, GET_INTENSITY, GET_PARAMETER,
                                SET_MODE, SET_INTENSITY, SET_PARAMETER };

  typedef struct {
  instructionClass instructionEnum;
  char* instructionString;
  } instruction_t;

  extern instruction_t instructions[];

}

#endif
