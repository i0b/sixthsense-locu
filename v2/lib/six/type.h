#ifndef TYPE_H
#define TYPE_H

#define REQUEST_RESPONSE_PACKET_LENGTH 1024
#define MAX_ELEMENTS 16
#define BLEMini Serial1

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

namespace six {

  enum class actuatorType { VIBRATION, PRESSURE, TEMPERATURE, ELECTRIC };
  extern const char* actuatorTypeString[];
  enum class executionMode { NONE, OFF, VIBRATION, HEARTBEAT, ROTATION, PRESSURE, TEMPERATURE, ELECTRO };
  extern const char* executionModeString[];

  enum class instructions { LIST, PING_DEVICE, DEMONSTRATE_DISCONNECT, GET_MODE, GET_INTENSITY, GET_PARAMETER,
                            SET_MODE, SET_INTENSITY, SET_PARAMETER };

}

#endif
