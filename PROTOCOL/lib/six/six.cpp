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
  const uint8_t version_major = 0;
  const uint8_t version_minor = 1;

  typedef enum { EMPTY, INT, STRING } value_t;
  char* parse_next ( char* packet_segment, size_t* segment_len, char* segment_type  );
  int set_packet_body ( response_packet_t* packet, value_t type, const char* value_information, const char* char_value, int int_value );
  int create_response_packet ( response_packet_t* packet, status::status_type status );


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
      packet->command.instruction = six::LIST;
    }

    else {
      char* uuid = raw_packet;
      size_t uuid_len;

      // read uuid
      raw_packet = parse_next ( raw_packet, &uuid_len, "uuid" );

      if ( raw_packet == 0 ) {
        return -1;
      }
    
      *packet_len -= uuid_len;

      // TODO if uuid no correct number, return -1
      int uuid_int = atoi ( uuid );

      if ( uuid_int < 0 || uuid_int > 255 ) {
        return -1;
      }

      packet->command.uuid = (uint8_t) uuid_int;

      if ( strncasecmp ( "GM", command, command_len ) == 0 ) {
        packet->command.instruction = six::GET_MODE;
      }
      else if ( strncasecmp ( "GV", command, command_len ) == 0 ) {
        packet->command.instruction = six::GET_INTENSITY;
      }
      else if ( strncasecmp ( "GP", command, command_len ) == 0 ) {
        packet->command.instruction = six::GET_PARAMETER;
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

        packet->command.value = value;
        packet->command.value_len = value_len;

        if ( strncasecmp ( "SM", command, command_len ) == 0 ) {
          packet->command.instruction = six::SET_MODE;
        }
        else if ( strncasecmp ( "SV", command, command_len ) == 0 ) {
          packet->command.instruction = six::SET_INTENSITY;
        }
        else if ( strncasecmp ( "SP", command, command_len ) == 0 ) {
          packet->command.instruction = six::SET_PARAMETER;
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

    packet->version_major = (uint8_t) major_int;
    packet->version_minor = (uint8_t) minor_int;

    char serial_buf[80];

    snprintf( serial_buf, sizeof ( serial_buf ), "version: %d.%d\r\n\r\n", packet->version_major, packet->version_minor );
    Serial.print( serial_buf );
    
    if ( *packet_len != 0 ) {
      // actually it should always be zero - the last string (minor) might be corrupt though
      return -1;
    }
    
    return 0;
  }


// ---------------------------------------------------------------------------------------------------------------------
//
//
  int eval_command ( request_packet_t* request, response_packet_t* response ) {

    if ( six::version_major != request->version_major || six::version_minor != request->version_minor ) {
       return -1;
    }



    // ---------------- LIST ACTUATORS --------------------------
    if ( request->command.instruction == six::LIST ) {
      /*
        char* description;
        uint8_t number_pins;
        uint8_t pins [ MAX_VAL_PINS ];
        actuator_type type;
      */

      strcpy ( response->body, "" );
      response->body_len = 0;

      for ( uint8_t uuid = 0; uuid < NUMBER_ACTUATORS; uuid++ ) {
        response->body_len += snprintf ( response->body + strlen ( response->body ), 
            REQUEST_RESPONSE_PACKET_LEN - response->body_len, 
            
            "uuid: %d\r\n"
            "description: %s\r\n"
            "number pins: %d\r\n"
            "pins: [",
            uuid,
            execute::executor [ uuid ].actuator->description,
            execute::executor [ uuid ].actuator->number_pins
          );

        
        for ( uint8_t pin = 0; pin < execute::executor [ uuid ].actuator->number_pins; pin++ ) {
          response->body_len += snprintf ( response->body + strlen ( response->body ), 
              REQUEST_RESPONSE_PACKET_LEN - response->body_len, 
              " %d",
              execute::executor [ uuid ].actuator->pins [ pin ]
            );
        }
        
        response->body_len += snprintf ( response->body + strlen ( response->body ), 
            REQUEST_RESPONSE_PACKET_LEN - response->body_len, 
            
            "]\r\n"
            "actuator type: %s\r\n"
            "\r\n",
            actuator::TYPE_STRING [ execute::executor [ uuid ].actuator->type ]
          );
      }

      response->body_len += snprintf ( response->body + strlen ( response->body ), 
          REQUEST_RESPONSE_PACKET_LEN - response->body_len, 
          "\r\n" );

      response->body_len = strlen ( response->body );


      response->status = status::status_description [ status::SIX_OK ];

      // TODO find the right place for this
      if ( response->status.status != status::SIX_OK ) {
        return -1;
      }
      
      response->version_major = six::version_major;
      response->version_minor = six::version_minor;

      // TODO find the right place for this
      send_response_packet ( response );
    }

    // ------------------ GET MODE ------------------------------
    else if ( request->command.instruction == six::GET_MODE ) {
      
      if ( set_packet_body ( response, STRING, "mode", 
           execute::EXECUTION_MODE_STRING [ execute::executor [ request->command.uuid ].mode ], 0 ) == 0 ) {

        create_response_packet ( response, status::SIX_OK );

        send_response_packet ( response );
      }

    }

    // ---------------- GET INTENSITY ---------------------------
    else if ( request->command.instruction == six::GET_INTENSITY ) {

      if ( set_packet_body ( response, INT, "intensity", NULL, 
            execute::executor [ request->command.uuid ].parameter [ 0 ] ) == 0 ) {

        create_response_packet ( response, status::SIX_OK );

        send_response_packet ( response );
      }

    }

    // TODO NOT COPY-PASTE!!
    // ---------------- GET PARAMETER ---------------------------
    else if ( request->command.instruction == six::GET_PARAMETER ) {
      
      if ( set_packet_body ( response, INT, "parameter", NULL, 
            execute::executor [ request->command.uuid ].parameter [ 1 ] ) == 0 ) {

        create_response_packet ( response, status::SIX_OK );

        send_response_packet ( response );
      }

    }



    // -------------------- SET MODE ----------------------------
    else if ( request->command.instruction == six::SET_MODE ) {
      execute::execution_mode mode = execute::OFF;

      if ( strncasecmp ( "BEAT", request->command.value, request->command.value_len ) == 0 ) {
        mode = execute::HEARTBEAT;
      }
      else if ( strncasecmp ( "ROT", request->command.value, request->command.value_len ) == 0 ) {
        mode = execute::ROTATION;
      }
      else if ( strncasecmp ( "VIB", request->command.value, request->command.value_len ) == 0 ) {
        mode = execute::VIBRATION;
      }
      else if ( strncasecmp ( "OFF", request->command.value, request->command.value_len ) == 0 ) {
        mode = execute::OFF;
      }
      else {
        return -1;
      }
      
      execute::set_mode ( request->command.uuid, mode );


      set_packet_body ( response, EMPTY, NULL, NULL , 0 );
      create_response_packet ( response, status::SIX_OK );
      send_response_packet ( response );

      return 0;
    }
    
    // -------------------- SET INTENSITY -----------------------
    else if ( request->command.instruction == six::SET_INTENSITY ) {
      int intensity = atoi ( request->command.value );
      //TODO check if parameter valide
      execute::set_intensity ( request->command.uuid, intensity );

      set_packet_body ( response, EMPTY, NULL, NULL , 0 );
      create_response_packet ( response, status::SIX_OK );
      send_response_packet ( response );

      return 0;
    }

    // -------------------- SET PARAMETER -----------------------
    else if ( request->command.instruction == six::SET_PARAMETER ) {
      int parameter = atoi ( request->command.value );
      //TODO check if parameter valide
      execute::set_parameter ( request->command.uuid, parameter );

      set_packet_body ( response, EMPTY, NULL, NULL , 0 );
      create_response_packet ( response, status::SIX_OK );
      send_response_packet ( response );

      return 0;
    }
  }


// ---------------------------------------------------------------------------------------------------------------------
//
//
  int send_response_packet ( response_packet_t* packet ) {
    Serial.print ( packet->status.CODE );
    Serial.print ( " " );
    Serial.print ( packet->status.DESCRIPTION );
    Serial.print ( " " );
    Serial.print ( "SIX/" );
    Serial.print ( packet->version_major );
    Serial.print ( "." );
    Serial.print ( packet->version_minor );
    Serial.print ( "\r\n" );
    Serial.print ( "\r\n" );

    Serial.print ( packet->body );
    Serial.print ( "\r\n" );


    Serial.print ( "\r\n" );
    Serial.print ( "\r\n" );

    return 0;
  }



// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
  
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

  int create_response_packet ( response_packet_t* packet, status::status_type status ) {

    packet->version_major = six::version_major;
    packet->version_minor = six::version_minor;
    //, size_t body_len ) {
    packet->status = status::status_description [ status ];

    // TODO find the right place for this
    //if ( packet->status.status != status::SIX_OK ) {
    //  return -1;
    //}

    return 0;
  }

  int set_packet_body ( response_packet_t* packet, value_t type, const char* value_information, const char* char_value, int int_value ) {

    strcpy ( packet->body, "" );
    packet->body_len = 0;
    
    if ( type == EMPTY ) {
      return 0;
    }

    else if ( type == INT ) {
      packet->body_len += snprintf (
          packet->body + strlen ( packet->body ),
          REQUEST_RESPONSE_PACKET_LEN - packet->body_len,
          "%s: %d\r\n\r\n",
          value_information,
          int_value
        );
    }
    
    else if ( type == STRING ) {
      packet->body_len += snprintf (
          packet->body + strlen ( packet->body ),
          REQUEST_RESPONSE_PACKET_LEN - packet->body_len,
          "%s: %s\r\n\r\n",
          value_information,
          char_value
        );
    }

    packet->body_len = strlen ( packet->body );

    return 0;
  }

}
