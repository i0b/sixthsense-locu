#ifndef PARSER_H
#define PARSER_H

#include "executor.h"
#include "status.h"
#include "type.h"
#include <stddef.h>
#include <stdint.h>

namespace six {

  class Parser {
    public:
      Parser(Executor* executor);
      void reset();
      void append(char c);
      int parseCommand();
      int parseCommand(char* rawPacket, size_t* packetLength);
      int evaluateCommand();

    private:
      typedef struct {
        instructions instruction;
        uint8_t id;
        char* value;
        size_t valueLength;
      } _command_t;

      typedef struct {
        uint8_t versionMajor;
        uint8_t versionMinor;
        _command_t command;
      } _requestPacket_t;

      typedef struct {
        uint8_t versionMajor;
        uint8_t versionMinor;
        six::status_t status;
        char* body;
        size_t bodyLength;
      } _responsePacket_t;
      
      typedef enum { EMPTY, INT, STRING } _value_t;

      char _packetData [ REQUEST_RESPONSE_PACKET_LENGTH ];
      size_t _packetLength;

      _requestPacket_t _requestPacket;
      _responsePacket_t _responsePacket;

      char* _parseNext(char* packetSegment, size_t* packetLength, size_t* segmentLength, char* segmentType);
      int _setPacketBody(_value_t type, const char* valueInformation, const char* charValue, int intValue);
      int _createResponsePacket(six::statusType status);
      int _sendResponsePacket();

      Executor* _executor;
  };

}

#endif
