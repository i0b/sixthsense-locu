#ifndef SIX_H
#define SIX_H

#include "status.h"
#include <stddef.h>
#include <stdint.h>

#define FOREACH_INSTRUCTION(COMMAND)    \
        COMMAND(LIST)                   \
        COMMAND(PING_DEVICE)            \
        COMMAND(DEMONSTRATE_DISCONNECT) \
        COMMAND(INSTRUCTIONS)           \
        COMMAND(GET_MODE)               \
        COMMAND(GET_INTENSITY)          \
        COMMAND(GET_PARAMETER)          \
        COMMAND(SET_MODE)               \
        COMMAND(SET_INTENSITY)          \
        COMMAND(SET_PARAMETER)          \

#ifndef GENERATOR
#define GENERATOR

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#endif

//#include <AltSoftSerial.h>

/*
// For UNO, AltSoftSerial library is required, please get it from:
// http://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)  
  AltSoftSerial BLEMini;
#else
*/

#define BLEMini Serial1
/*
#endif
*/
#define REQUEST_RESPONSE_PACKET_LEN 1024

namespace six {
  class Parse {
    //extern static const uint8_t VERSION_MAJOR;
    //extern static const uint8_t VERSION_MINOR;

    extern static const char* INSTRUCTIONS_STRING[];
      
    typedef enum { FOREACH_INSTRUCTION ( GENERATE_ENUM ) } instruction_t;

    typedef struct {
      instruction_t instruction;
      uint8_t uid;
      char* value;
      size_t valueLength;
    } command_t;

    typedef struct {
      uint8_t versionMajor;
      uint8_t versionMinor;

      command_t command;
    } requestPacket_t;

    typedef struct {
      uint8_t versionMajor;
      uint8_t versionMinor;

      status::status_t status;
      
      char* body;
      size_t bodyLength;
    } responsePacket_t;

    typedef enum { EMPTY, INT, STRING } value_t;

    public:
      // upon successful completion, these functions shall return 0. Otherwise, these functions shall return −1
      static int parseCommand ( char* rawPacket, size_t packetLength );
      static int evaluateCommand ();

      static const uint8_t versionMajor = 0;
      static const uint8_t versionMinor = 1;

    private:
      static char _packetData [ requestResponsePacketLength ];
      static size_t _packetLength;
      static requestPacket_t _requestPacket;
      static responsePacket_t _responsePacket;

      static char* _parseNext ( char* packetSegment, size_t* packetLength, size_t* segmentLength, char* segmentType  );
      static int _setPacketBody ( value_t type, const char* valueInformation, const char* charValue, int intValue );
      static int _createResponsePacket ( status::statusType status );
      static int _sendResponsePacket ();

  }

}

#endif
