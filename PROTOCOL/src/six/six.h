#ifndef SIX_H
#define SIX_H

#include "six-execute.h"

typedef struct
{
  byte VERSION_MAJOR;
  byte VERSION_MINOR;

  unsigned int STATUS;
  
  unsigned int LEN;
  unsigned char* BODY;
} six_packet_t;

// upon successful completion, these functions shall return 0. Otherwise, these functions shall return −1
int parse_command ( char* raw_packet, size_t packet_len, environment_t* env );
int create_request_packet ( six_packet_t* packet, byte min, byte maj, byte uuid, byte parameter_no, int val );
int create_reply_packet ( six_packet_t* packet, byte min, byte maj, unsigned int status, char* body, size_t body_len );
int send_packet ( six_packet_t* packet );

#endif
