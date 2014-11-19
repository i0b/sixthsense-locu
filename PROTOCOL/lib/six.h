#ifndef SIX_H
#define SIX_H

#include "six-execute.h"

typedef struct six_packet
{
  byte VERSION_MAJOR;
  byte VERSION_MINOR;

  unsigned int UUID;
  int VALUE;

  byte PARAM_NO;
};

// upon successful completion, these functions shall return 0. Otherwise, these functions shall return −1
int parse_command ( char* raw_packet, size_t packet_len, environment* env );
int send_request_packet ( byte min, byte maj, int uuid, int val, byte parno );
int send_reply_packet ( byte min, byte maj, unsigned int status, byte reply_type, char* data, size_t data_len );

#endif
