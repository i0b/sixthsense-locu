#include "six.h"
#include "execute.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <SPI.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// returns new position of inside the packet after the space
char* parse_next ( char* packet_segment, size_t* segment_len, char* segment_type  ) {
    char* space = strchr ( packet_segment, ' ' );
   
    if ( space == NULL ) {
     return 0;
    }

    else {
     *segment_len = space - packet_segment;
    }

    if ( *segment_len < 1 ) {
      return 0;
    }

    *space = '\0';

    // output example: "protocol: six/0.1"
    Serial.print ( segment_type );
    Serial.print ( ": " );
    Serial.println ( package_segment );

    return space + 1;
}

int parse_command ( char* raw_packet, size_t* packet_len, six_request_packet_t* packet ) {

  // parse command - example GETV
  char* command = raw_packet;
  size_t command_len;

  raw_packet = parse_next ( raw_packet, &command_len, "command" );

  if ( raw_packet == 0 ) {
    return -1;
  }
  
  *packet_len -= command_len;
/*
  // commands all consist of 4 characters
  if ( command_len != 2 )
    return -1;
*/

  // evaluate command string
   
  if ( strncasecmp ( "LIST", command, command_len ) == 0 ) {
    packet->COMMAND.INSTRUCTION = command_t::LIST;
  }

  else {
    char* uuid = raw_packet;
    size_t uuid_len;

    // read UUID
    raw_packet = parse_next ( raw_packet, &uuid_len, "UUID" );

    if ( raw_packet == 0 ) {
      return -1;
    }
  
    *packet_len -= uuid_len;

    // TODO if uuid no correct number, return -1
    int uuid_int = atoi ( uuid );

    if ( uuid < 0 || uuid > 255 ) {
      return -1;
    }

    packet->COMMAND.UUID = (uint8_t) uuid_int;

    if ( strncasecmp ( "GV", command, command_len ) == 0 ) {
      packet->COMMAND.INSTRUCTION = command_t::GETV;
    }
    else if ( strncasecmp ( "GM", command, command_len ) == 0 ) {
      packet->COMMAND.INSTRUCTION = command_t::GETM;
    }
    else if ( strncasecmp ( "GP", command, command_len ) == 0 ) {
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
    
      *packet_len -= value_len;

      packet->COMMAND.VALUE = value;
      packet->COMMAND.VALUE_LEN = value_len;

      if ( strncasecmp ( "SV", command, command_len ) == 0 ) {
        packet->COMMAND.INSTRUCTION = command_t::SETV;
      }
      else if ( strncasecmp ( "SM", command, command_len ) == 0 ) {
        packet->COMMAND.INSTRUCTION = command_t::SETM;
      }
      else if ( strncasecmp ( "SP", command, command_len ) == 0 ) {
        packet->COMMAND.INSTRUCTION = command_t::SETP;
      }
    }
  }

  // compare version - example: SIX/0.1
  char* protocol_name = "SIX/";

  if ( strncasecmp ( protocol_name, raw_packet, strlen ( protocol_name ) ) == 0 ) {
    raw_packet += strlen ( protocol_name );
  }
  else {
    return -1;
  }
  
  *packet_len -= strlen ( protocol_name );

  char* dot = strchr ( raw_packet, '.' );

  if ( dot == NULL ) {
    return -1;
  }

  *dot = '\0';
  
  char* major = raw_packet;
  size_t major_len = strlen ( major );

  char* minor = dot + 1;
  size_t minor_len = strlen ( minor );
  

  *packet_len -= major_len + minor_len;


  // TODO if major no correct number, return -1
  int major_int = atoi ( major );

  if ( major_int < 0 || major_int > 255 ) {
    return -1;
  }

  // TODO if minor no correct number, return -1
  int minor_int = atoi ( minor );

  if ( minor_int < 0 || minor_int > 255 ) {
    return -1;
  }

  packet->VERSION_MAJOR = (uint8_t) major_int;
  packet->VERSION_MINOR = (uint8_t) minor_int;

  char serial_buf[80];

  snprintf( serial_buf, sizeof ( serial_buf ), "version: %d.%d\r\n", packet->VERSION_MAJOR, packet->VERSION_MINOR );
  Serial.print( serial_buf );
  
  if ( *packet_len != 0 ) {
    // actually it should always be zero - the last string (minor) might be corrupt though
    return -1;
  }
  
  return 0;
}

int eval_command ( six_request_packet_t* packet ) {

  if ( six::VERSION_MAJOR != packet->VERSION_MAJOR || six::VERSION_MINOR != packet->VERSION_MINOR ) {
     return -1;
  }

  if ( packet->COMMAND.INSTRUCTION == command_t::SETM ) {
    execute::execution_mode mode = execute::OFF;

    if ( strncasecmp ( "BEAT", packet->COMMAND.VALUE, packet->COMMAND.VALUE_LEN ) == 0 ) {
      mode = execute::HEARTBEAT;
    }
    else if ( strncasecmp ( "ROT", packet->COMMAND.VALUE, packet->COMMAND.VALUE_LEN ) == 0 ) {
      mode = execute::ROTATION;
    }
    else if ( strncasecmp ( "VIB", packet->COMMAND.VALUE, packet->COMMAND.VALUE_LEN ) == 0 ) {
      mode = execute::VIBRATION;
    }
    else if ( strncasecmp ( "OFF", packet->COMMAND.VALUE, packet->COMMAND.VALUE_LEN ) == 0 ) {
      mode = execute::OFF;
    }
    else {
      return -1;
    }
    
    execute::set_mode ( packet->COMMAND.UUID, mode );

    return 0;
  }
  
  else if ( packet->COMMAND.INSTRUCTION == command_t::SETV ) {
    int intensity = atoi ( packet->COMMAND.VALUE );
    //TODO check if parameter valide
    execute::set_intensity ( intensity );

    return 0;
  }

  else if ( packet->COMMAND.INSTRUCTION == command_t::SETP ) {
    int parameter = atoi ( packet->COMMAND.VALUE );
    //TODO check if parameter valide
    execute::set_parameter ( parameter );

    return 0;
  }
}

int create_reply_packet ( six_response_packet_t* packet, uint8_t min, uint8_t maj, uint8_t status, char* body, size_t body_len ) {
  return 0;
}

int send_reqply_packet ( six_response_packet_t* packet ) {
  return 0;
}
