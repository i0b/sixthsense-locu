#include "six-execute.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAJOR 0
#define MINOR 1

char serial_buf[50];

char* parse_next ( char* packet_segment, size_t* segment_len  )
{
    char* space = strchr ( packet_segment, ' ' );
   
    if ( space == NULL )
     return 0;
    else
     *segment_len = space - packet_segment;

    if ( *segment_len < 1 )
      return 0;

    *space = '\0';
    snprintf( serial_buf, sizeof(serial_buf), "read: %s\n", packet_segment, *segment_len );
    //Serial.write( serial_buf );

    return space + 1;
}

int parse_command ( char* raw_packet, size_t packet_len, environment_t* env )
{
  // parse command - example GETV
  char* command = raw_packet;
  size_t command_len;

  raw_packet = parse_next ( raw_packet, &command_len );

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
    raw_packet = parse_next ( raw_packet, &uuid_len );

    if ( raw_packet == 0 )
      return -1;

    int INT_UUID = atoi(uuid);

    if ( strncasecmp ( "GETV", command, command_len ) == 0 );
    else if ( strncasecmp ( "GETM", command, command_len ) == 0 );
    else if ( strncasecmp ( "GETP", command, command_len ) == 0 );

    else
    {
      // read value
      value = raw_packet;
      raw_packet = parse_next ( raw_packet, &value_len );
    
      if ( raw_packet == 0 )
        return -1;

      int INT_VALUE = atoi(value);

      if ( strncasecmp ( "SETV", command, command_len ) == 0 );
      else if ( strncasecmp ( "SETM", command, command_len ) == 0 );
      else if ( strncasecmp ( "SETP", command, command_len ) == 0 );
    
    }
  }

  // compare version - example: SIX/0.1
  char version[12];

  int version_len = snprintf ( version, 12, "SIX/%d.%d", MAJOR, MINOR );
  
  char* version_match_pos;

  if ( version_len < 0 )
     return -1;
  else
    version_match_pos = strstr ( raw_packet, version );
  
  snprintf( serial_buf, sizeof(serial_buf), "received version: %s, internal version: %s\n", raw_packet, version );
  //Serial.write( serial_buf );

  if ( version_match_pos != raw_packet )
    return -1;
 
  return 0;
}
/*
int create_request_packet ( six_packet_t* packet, byte min, byte maj, byte uuid, byte parameter_no, int val ) { return 0; }
int create_reply_packet ( six_packet_t* packet, byte min, byte maj, unsigned int status, char* body, size_t body_len ) { return 0; }


int send_packet ( six_packet_t* packet )
{
  char header[50];
  snprintf( header, sizeof(header), "SIX\/%d.%d %d\r\n%s", packet->MAJOR, packet->MINOR );
  //Serial.write( header );

  //Serial.write( packet->body );

  return 0; 
}

*/
