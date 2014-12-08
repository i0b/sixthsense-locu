#ifndef SIX_H
#define SIX_H

#include "status.h"
#include <stddef.h>
#include <stdint.h>

#define REQUEST_RESPONSE_PACKET_LEN 1024

namespace six {

  extern const uint8_t VERSION_MAJOR;
  extern const uint8_t VERSION_MINOR;
    
  typedef enum { LIST, INSTRUCTIONS, GET_MODE, GET_INTENSITY, GET_PARAMETER, SET_MODE, SET_INTENSITY, SET_PARAMETER } instruction_t;

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
  int parse_command ( char* raw_packet, size_t* packet_len, request_packet_t* packet );
  int eval_command ( request_packet_t* request, response_packet_t* response );
  int send_response_packet ( response_packet_t* packet );

}

#endif
