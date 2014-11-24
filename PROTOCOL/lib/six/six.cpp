#include "six.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <SPI.h>

char serial_buf[80];

char* parse_next ( char* packet_segment, size_t* segment_len, char* segment_type  )
{
    char* space = strchr ( packet_segment, ' ' );
   
    if ( space == NULL )
     return 0;
    else
     *segment_len = space - packet_segment;

    if ( *segment_len < 1 )
      return 0;

    *space = '\0';
    snprintf( serial_buf, sizeof ( serial_buf ), "%s: %s\r\n", segment_type, packet_segment, *segment_len );
    Serial.write( serial_buf );

    return space + 1;
}

int parse_command ( char* raw_packet, size_t* packet_len, six_request_packet_t* packet )
{
  *packet_len = 0;

  // parse command - example GETV
  char* command = raw_packet;
  size_t command_len;

  raw_packet = parse_next ( raw_packet, &command_len, "command" );

  if ( raw_packet == 0 )
    return -1;

  // commands all consist of 4 characters
  if ( command_len != 4 )
    return -1;

  // evaluate command string
    
  if ( strncasecmp ( "LIST", command, command_len ) == 0 )
  {
    packet->COMMAND.INSTRUCTION = command_t::LIST;
  }

  else
  {
    char* uuid = raw_packet;
    size_t uuid_len;

    // read UUID
    raw_packet = parse_next ( raw_packet, &uuid_len, "UUID" );

    if ( raw_packet == 0 )
      return -1;

    uint8_t uuid_int = (uint8_t) atoi(uuid);
    packet->COMMAND.UUID = uuid_int;

    if ( strncasecmp ( "GETV", command, command_len ) == 0 ) {
      packet->COMMAND.INSTRUCTION = command_t::GETV;
    }
    else if ( strncasecmp ( "GETM", command, command_len ) == 0 ) {
      packet->COMMAND.INSTRUCTION = command_t::GETM;
    }
    else if ( strncasecmp ( "GETP", command, command_len ) == 0 ) {
      packet->COMMAND.INSTRUCTION = command_t::GETP;
    }

    else
    {
      char* value;
      size_t value_len;

      // read value
      value = raw_packet;
      raw_packet = parse_next ( raw_packet, &value_len, "value" );
    
      if ( raw_packet == 0 )
        return -1;

      packet->COMMAND.VALUE = value;
      packet->COMMAND.VALUE_LEN = value_len;

      if ( strncasecmp ( "SETV", command, command_len ) == 0 ) {
        packet->COMMAND.INSTRUCTION = command_t::SETV;
      }
      else if ( strncasecmp ( "SETM", command, command_len ) == 0 ) {
        packet->COMMAND.INSTRUCTION = command_t::SETM;
      }
      else if ( strncasecmp ( "SETP", command, command_len ) == 0 ) {
        packet->COMMAND.INSTRUCTION = command_t::SETP;
      }
    }
  }

  // compare version - example: SIX/0.1
  char* protocol_name = "SIX/";

  if ( strncasecmp ( protocol_name, raw_packet, strlen ( protocol_name ) ) == 0 )
    raw_packet += strlen ( protocol_name );
  else
    return -1;
  
  char* dot = strchr ( raw_packet, '.' );

  if ( dot == NULL )
    return -1;

  *dot = '\0';

  char* minor_str = dot + 1;

  packet->VERSION_MAJOR = (uint8_t) atoi ( raw_packet );
  packet->VERSION_MINOR = (uint8_t) atoi ( minor_str );

  snprintf( serial_buf, sizeof ( serial_buf ), "version: %d.%d\r\n", packet->VERSION_MAJOR, packet->VERSION_MINOR );
  Serial.write( serial_buf );
  
  return 0;
}

int eval_command ( six_request_packet_t* packet, environment_t* env )
{
  if ( env->VERSION_MAJOR != packet->VERSION_MAJOR || env->VERSION_MINOR != packet->VERSION_MINOR )
     return -1;

  if ( packet->COMMAND.INSTRUCTION == command_t::SETM ) {
    if ( strncasecmp ( "BEAT", packet->COMMAND.VALUE, packet->COMMAND.VALUE_LEN ) == 0 ) {
      env->MODE = environment_t::HEARTBEAT;
    }
    else if ( strncasecmp ( "ROT", packet->COMMAND.VALUE, packet->COMMAND.VALUE_LEN ) == 0 ) {
      env->MODE = environment_t::ROTATION;
    }
    else if ( strncasecmp ( "VIB", packet->COMMAND.VALUE, packet->COMMAND.VALUE_LEN ) == 0 ) {
      env->MODE = environment_t::VIBRATION;
    }
    else if ( strncasecmp ( "OFF", packet->COMMAND.VALUE, packet->COMMAND.VALUE_LEN ) == 0 ) {
      env->MODE = environment_t::OFF;
    }
    else {
      return -1;
    }

    return 0;
  }
  else if ( packet->COMMAND.INSTRUCTION == command_t::SETP ) {
    if ( env->MODE == environment_t::HEARTBEAT ) {
      env->HEARTBEAT_DELAY_PAUSE = (unsigned int) atoi( packet->COMMAND.VALUE );
    }
    else if ( env->MODE == environment_t::ROTATION ) {
      env->ROTATION_SPEED = (unsigned int) atoi( packet->COMMAND.VALUE );
    }
    else if ( env->MODE == environment_t::VIBRATION ) {
      env->VIBRATION_LEVEL = (unsigned int) atoi( packet->COMMAND.VALUE );
    }

    return 0;
  }

  return -1;
}

