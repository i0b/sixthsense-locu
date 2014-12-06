#ifndef SIX_H
#define SIX_H

#include <stddef.h>
#include <stdint.h>


uint8_t VERSION_MAJOR = 0;
uint8_t VERSION_MINOR = 1;

typedef struct
{
  enum { LIST, GETV, GETM, GETP, SETV, SETM, SETP } INSTRUCTION;
  uint8_t UUID;
  char* VALUE;
  size_t VALUE_LEN;
} command_t;

typedef struct
{
  uint8_t VERSION_MAJOR;
  uint8_t VERSION_MINOR;

  unsigned int STATUS;
  
  char* BODY;
  size_t BODY_LEN;
} six_response_packet_t;

typedef struct
{
  uint8_t VERSION_MAJOR;
  uint8_t VERSION_MINOR;

  command_t COMMAND;
} six_request_packet_t;


// upon successful completion, these functions shall return 0. Otherwise, these functions shall return −1
int parse_command ( char* raw_packet, size_t* packet_len, six_request_packet_t* packet );
int eval_command ( six_request_packet_t* packet );

int create_reply_packet ( six_response_packet_t* packet, uint8_t min, uint8_t maj, uint8_t status, char* body, size_t body_len );
int send_reqply_packet ( six_response_packet_t* packet );

#endif
