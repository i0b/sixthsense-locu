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

  extern const uint8_t VERSION_MAJOR;
  extern const uint8_t VERSION_MINOR;
    
  typedef enum { FOREACH_INSTRUCTION ( GENERATE_ENUM ) } instruction_t;

  extern const char* INSTRUCTIONS_STRING[];

  typedef struct {
    instruction_t instruction;
    uint8_t uuid;
    char* value;
    size_t value_len;
  } command_t;

  typedef struct {
    uint8_t version_major;
    uint8_t version_minor;

    status::status_t status;
    
    char* body;
    size_t body_len;
  } response_packet_t;

  typedef struct {
    uint8_t version_major;
    uint8_t version_minor;

    command_t command;
  } request_packet_t;


  // upon successful completion, these functions shall return 0. Otherwise, these functions shall return −1
  int parse_command ( char* raw_packet, size_t* packet_len, request_packet_t* request, response_packet_t* response );
  int evaluate_command ( request_packet_t* request, response_packet_t* response );
  int send_response_packet ( response_packet_t* packet );

}

#endif
