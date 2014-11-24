#include "six-execute.h"
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

int parse_command ( char* raw_packet, size_t* packet_len, environment_t* env )
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
  char* uuid = raw_packet;
  size_t uuid_len;
  
  char* value;
  size_t value_len;

  //TODO evaluation, set mode
  if ( strncasecmp ( "LIST", command, command_len ) == 0 )
  {
    // set command type to LIST
  }

  else
  {
    // read UUID
    raw_packet = parse_next ( raw_packet, &uuid_len, "UUID" );

    if ( raw_packet == 0 )
      return -1;

    int INT_UUID = atoi(uuid);

    if ( strncasecmp ( "GETV", command, command_len ) == 0 ) {
      if ( env->MODE == environment_t::HEARTBEAT )
        env->HEARTBEAT_FREQ = 100;
    }
    else if ( strncasecmp ( "GETM", command, command_len ) == 0 ) {
      env->MODE = environment_t::ROTATION;
    }
    else if ( strncasecmp ( "GETP", command, command_len ) == 0 ) {
      env->MODE = environment_t::HEARTBEAT;
    }

    else
    {
      // read value
      value = raw_packet;
      raw_packet = parse_next ( raw_packet, &value_len, "value" );
    
      if ( raw_packet == 0 )
        return -1;

      int INT_VALUE = atoi(value);

      if ( strncasecmp ( "SETV", command, command_len ) == 0 )
        if ( env->MODE == environment_t::ROTATION)
          env->HEARTBEAT_FREQ = 100;
      else if ( strncasecmp ( "SETM", command, command_len ) == 0 )
        env->MODE = environment_t::ROTATION;
      else if ( strncasecmp ( "SETP", command, command_len ) == 0 )
        env->MODE = environment_t::ROTATION;
    
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

  int major = atoi ( raw_packet );
  int minor = atoi ( minor_str );

  snprintf( serial_buf, sizeof ( serial_buf ), "version: %d.%d\r\n", major, minor );
  Serial.write( serial_buf );
  
  if ( env->MAJOR != major || env->MINOR != minor )
     return -1;
 
  return 0;
}

