#include "six.h"
#include "actuator.h"
#include "execute.h"
#include "status.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <SPI.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

namespace six {
  const uint8_t VERSION_MAJOR = 0;
  const uint8_t VERSION_MINOR = 1;

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
      Serial.println ( packet_segment );

      return space + 1;
  }

  int parse_command ( char* raw_packet, size_t* packet_len, request_packet_t* packet ) {

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

      if ( uuid_int < 0 || uuid_int > 255 ) {
        return -1;
      }

      packet->COMMAND.UUID = (uint8_t) uuid_int;

      if ( strncasecmp ( "GM", command, command_len ) == 0 ) {
        packet->COMMAND.INSTRUCTION = command_t::GET_MODE;
      }
      else if ( strncasecmp ( "GV", command, command_len ) == 0 ) {
        packet->COMMAND.INSTRUCTION = command_t::GET_INTENSITY;
      }
      else if ( strncasecmp ( "GP", command, command_len ) == 0 ) {
        packet->COMMAND.INSTRUCTION = command_t::GET_PARAMETER;
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

        if ( strncasecmp ( "SM", command, command_len ) == 0 ) {
          packet->COMMAND.INSTRUCTION = command_t::SET_MODE;
        }
        else if ( strncasecmp ( "SV", command, command_len ) == 0 ) {
          packet->COMMAND.INSTRUCTION = command_t::SET_INTENSITY;
        }
        else if ( strncasecmp ( "SP", command, command_len ) == 0 ) {
          packet->COMMAND.INSTRUCTION = command_t::SET_PARAMETER;
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

    snprintf( serial_buf, sizeof ( serial_buf ), "version: %d.%d\r\n\r\n", packet->VERSION_MAJOR, packet->VERSION_MINOR );
    Serial.print( serial_buf );
    
    if ( *packet_len != 0 ) {
      // actually it should always be zero - the last string (minor) might be corrupt though
      return -1;
    }
    
    return 0;
  }

  int eval_command ( request_packet_t* request, response_packet_t* response ) {

    if ( six::VERSION_MAJOR != request->VERSION_MAJOR || six::VERSION_MINOR != request->VERSION_MINOR ) {
       return -1;
    }



    // ---------------- LIST ACTUATORS --------------------------
    if ( request->COMMAND.INSTRUCTION == command_t::LIST ) {
      /*
        char* description;
        uint8_t number_pins;
        uint8_t pins [ MAX_VAL_PINS ];
        actuator_type type;
      */

      strcpy ( response->BODY, "" );
      response->BODY_LEN = 0;

      for ( uint8_t uuid = 0; uuid < NUMBER_ACTUATORS; uuid++ ) {
        response->BODY_LEN += snprintf ( response->BODY + strlen ( response->BODY ), 
            REQUEST_RESPONSE_PACKET_LEN - response->BODY_LEN, 
            
            "uuid: %d\r\n"
            "description: %s\r\n"
            "number pins: %d\r\n"
            "pins: [",
            uuid,
            execute::executor [ uuid ].actuator->description,
            execute::executor [ uuid ].actuator->number_pins
          );

        
        for ( uint8_t pin = 0; pin < execute::executor [ uuid ].actuator->number_pins; pin++ ) {
          response->BODY_LEN += snprintf ( response->BODY + strlen ( response->BODY ), 
              REQUEST_RESPONSE_PACKET_LEN - response->BODY_LEN, 
              " %d",
              execute::executor [ uuid ].actuator->pins [ pin ]
            );
        }
        
        response->BODY_LEN += snprintf ( response->BODY + strlen ( response->BODY ), 
            REQUEST_RESPONSE_PACKET_LEN - response->BODY_LEN, 
            
            "]\r\n"
            "actuator type: %s\r\n"
            "\r\n",
            actuator::TYPE_STRING [ execute::executor [ uuid ].actuator->type ]
          );
      }

      response->BODY_LEN += snprintf ( response->BODY + strlen ( response->BODY ), 
          REQUEST_RESPONSE_PACKET_LEN - response->BODY_LEN, 
          "\r\n" );

      response->BODY_LEN = strlen ( response->BODY );


      response->status = status::status_description [ status::SIX_OK ];

      // TODO find the right place for this
      if ( response->status.status != status::SIX_OK ) {
        return -1;
      }
      
      response->VERSION_MAJOR = six::VERSION_MAJOR;
      response->VERSION_MINOR = six::VERSION_MINOR;

      // TODO find the right place for this
      send_response_packet ( response );

    }


    // ---------------- GET INTENSITY ---------------------------
    else if ( request->COMMAND.INSTRUCTION == command_t::GET_INTENSITY ) {
      strcpy ( response->BODY, "" );
      response->BODY_LEN = 0;

      response->BODY_LEN += snprintf ( 
          response->BODY + strlen ( response->BODY ), 
          REQUEST_RESPONSE_PACKET_LEN - response->BODY_LEN, 
          "intensity: %d\r\n\r\n",
          execute::executor [ request->COMMAND.UUID ].parameter [ 0 ]
        );
        

      response->BODY_LEN = strlen ( response->BODY );

      response->status = status::status_description [ status::SIX_OK ];

      // TODO find the right place for this
      if ( response->status.status != status::SIX_OK ) {
        return -1;
      }
      
      response->VERSION_MAJOR = six::VERSION_MAJOR;
      response->VERSION_MINOR = six::VERSION_MINOR;

      // TODO find the right place for this
      send_response_packet ( response );
    }

    // TODO NOT COPY-PASTE!!
    // ---------------- GET PARAMETER ---------------------------
    else if ( request->COMMAND.INSTRUCTION == command_t::GET_PARAMETER ) {
      strcpy ( response->BODY, "" );
      response->BODY_LEN = 0;

      response->BODY_LEN += snprintf ( 
          response->BODY + strlen ( response->BODY ), 
          REQUEST_RESPONSE_PACKET_LEN - response->BODY_LEN, 
          "parameter: %d\r\n\r\n",
          execute::executor [ request->COMMAND.UUID ].parameter [ 1 ]
        );
        

      response->BODY_LEN = strlen ( response->BODY );

      response->status = status::status_description [ status::SIX_OK ];

      // TODO find the right place for this
      if ( response->status.status != status::SIX_OK ) {
        return -1;
      }
      
      response->VERSION_MAJOR = six::VERSION_MAJOR;
      response->VERSION_MINOR = six::VERSION_MINOR;

      // TODO find the right place for this
      send_response_packet ( response );
    }



    // -------------------- SET MODE ----------------------------
    else if ( request->COMMAND.INSTRUCTION == command_t::SET_MODE ) {
      execute::execution_mode mode = execute::OFF;

      if ( strncasecmp ( "BEAT", request->COMMAND.VALUE, request->COMMAND.VALUE_LEN ) == 0 ) {
        mode = execute::HEARTBEAT;
      }
      else if ( strncasecmp ( "ROT", request->COMMAND.VALUE, request->COMMAND.VALUE_LEN ) == 0 ) {
        mode = execute::ROTATION;
      }
      else if ( strncasecmp ( "VIB", request->COMMAND.VALUE, request->COMMAND.VALUE_LEN ) == 0 ) {
        mode = execute::VIBRATION;
      }
      else if ( strncasecmp ( "OFF", request->COMMAND.VALUE, request->COMMAND.VALUE_LEN ) == 0 ) {
        mode = execute::OFF;
      }
      else {
        return -1;
      }
      
      execute::set_mode ( request->COMMAND.UUID, mode );

      return 0;
    }
    
    else if ( request->COMMAND.INSTRUCTION == command_t::SET_INTENSITY ) {
      int intensity = atoi ( request->COMMAND.VALUE );
      //TODO check if parameter valide
      execute::set_intensity ( request->COMMAND.UUID, intensity );

      return 0;
    }

    else if ( request->COMMAND.INSTRUCTION == command_t::SET_PARAMETER ) {
      int parameter = atoi ( request->COMMAND.VALUE );
      //TODO check if parameter valide
      execute::set_parameter ( request->COMMAND.UUID, parameter );

      return 0;
    }
  }

  int create_response_packet ( response_packet_t* packet, uint8_t min, uint8_t maj, uint8_t status, char* body, size_t body_len ) {
    //, size_t body_len ) {
    return 0;
  }

  int send_response_packet ( response_packet_t* packet ) {
    Serial.print ( packet->status.CODE );
    Serial.print ( " " );
    Serial.print ( packet->status.DESCRIPTION );
    Serial.print ( " " );
    Serial.print ( "SIX/" );
    Serial.print ( packet->VERSION_MAJOR );
    Serial.print ( "." );
    Serial.print ( packet->VERSION_MINOR );
    Serial.print ( "\r\n" );
    Serial.print ( "\r\n" );

    Serial.print ( packet->BODY );
    Serial.print ( "\r\n" );


    Serial.print ( "\r\n" );
    Serial.print ( "\r\n" );

    return 0;
  }
}
