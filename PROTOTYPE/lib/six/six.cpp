#include "six.h"
#include "actuator.h"
#include "execute.h"
#include "status.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <SPI.h>
#include <RBL_nRF8001.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#ifndef GENERATE
#define GERERATE

#endif

namespace six {
  const uint8_t version_major = 0;
  const uint8_t version_minor = 1;

  typedef enum { EMPTY, INT, STRING } value_t;
  const char* INSTRUCTIONS_STRING[] = { FOREACH_INSTRUCTION ( GENERATE_STRING ) };

  char* parse_next ( char* packet_segment, size_t* packet_len, size_t* segment_len, char* segment_type  );
  int set_packet_body ( response_packet_t* packet, value_t type, const char* value_information, const char* char_value, int int_value );
  int create_response_packet ( response_packet_t* packet, status::status_type status );


  int parse_command ( char* raw_packet, size_t* packet_len, request_packet_t* request, response_packet_t* response ) {

    // parse command - example GETV
    char* command = raw_packet;
    size_t command_len;

    raw_packet = parse_next ( raw_packet, packet_len, &command_len, "command" );

    if ( raw_packet == 0 ) {
      set_packet_body ( response, EMPTY, NULL, NULL , 0 );
      create_response_packet ( response, status::SIX_ERROR_PARSING );
      send_response_packet ( response );

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
      request->command.instruction = six::LIST;
    }

    else {
      char* uuid = raw_packet;
      size_t uuid_len;

      // read uuid
      raw_packet = parse_next ( raw_packet, packet_len, &uuid_len, "uuid" );

      if ( raw_packet == 0 ) {
        set_packet_body ( response, EMPTY, NULL, NULL , 0 );
        create_response_packet ( response, status::SIX_ERROR_PARSING );
        send_response_packet ( response );

        return -1;
      }
    
      *packet_len -= uuid_len;

      // TODO if uuid no correct number, return -1
      int uuid_int = atoi ( uuid );

      if ( uuid_int < 0 || uuid_int > 255 ) {
        set_packet_body ( response, EMPTY, NULL, NULL , 0 );
        create_response_packet ( response, status::SIX_ERROR_PARSING );
        send_response_packet ( response );

        return -1;
      }

      request->command.uuid = (uint8_t) uuid_int;

      if ( strncasecmp ( "GM", command, command_len ) == 0 ) {
        request->command.instruction = six::GET_MODE;
      }
      else if ( strncasecmp ( "GV", command, command_len ) == 0 ) {
        request->command.instruction = six::GET_INTENSITY;
      }
      else if ( strncasecmp ( "GP", command, command_len ) == 0 ) {
        request->command.instruction = six::GET_PARAMETER;
      }

      else
      {
        char* value;
        size_t value_len;

        // read value
        value = raw_packet;
        raw_packet = parse_next ( raw_packet, packet_len, &value_len, "value" );
      
        if ( raw_packet == 0 ) {
          set_packet_body ( response, EMPTY, NULL, NULL , 0 );
          create_response_packet ( response, status::SIX_ERROR_PARSING );
          send_response_packet ( response );

          return -1;
        }
      
        *packet_len -= value_len;

        request->command.value = value;
        request->command.value_len = value_len;

        if ( strncasecmp ( "SM", command, command_len ) == 0 ) {
          request->command.instruction = six::SET_MODE;
        }
        else if ( strncasecmp ( "SV", command, command_len ) == 0 ) {
          request->command.instruction = six::SET_INTENSITY;
        }
        else if ( strncasecmp ( "SP", command, command_len ) == 0 ) {
          request->command.instruction = six::SET_PARAMETER;
        }
        else {
          set_packet_body ( response, EMPTY, NULL, NULL , 0 );
          create_response_packet ( response, status::SIX_COMMAND_NOT_FOUND );
          send_response_packet ( response );

          return -1;
        }
      }
    }

    // compare version - example: SIX/0.1
    char* protocol_name = "SIX/";

    if ( strncasecmp ( protocol_name, raw_packet, strlen ( protocol_name ) ) == 0 ) {
      raw_packet += strlen ( protocol_name );
    }
    else {
      set_packet_body ( response, EMPTY, NULL, NULL , 0 );
      create_response_packet ( response, status::SIX_ERROR_PARSING );
      send_response_packet ( response );

      return -1;
    }
    
    *packet_len -= strlen ( protocol_name );

    // TODO !!!!!!!!! replace STRCHR !!!!!!!!!!!!!!!!!
    //char* dot = strchr ( raw_packet, '.' );
    char* dot = (char*) memchr ( (void*)raw_packet, '.', *packet_len );

    if ( dot == NULL ) {
      set_packet_body ( response, EMPTY, NULL, NULL , 0 );
      create_response_packet ( response, status::SIX_ERROR_PARSING );
      send_response_packet ( response );

      return -1;
    }

    *dot = '\0';
    
    char* major = raw_packet;
    size_t major_len = strlen ( major );

    // TODO this still allows: LIST SIX/0.1somethingelse
    // USE SSCANF ( %d ) and check if string is empty afterwards!
    char* minor = dot + 1;
    size_t minor_len = strlen ( minor );
    

    *packet_len -= major_len + minor_len + 1; // 1 = '.'

    int major_int = atoi ( major );

    if ( major_int < 0 || major_int > 255 ) {
      set_packet_body ( response, EMPTY, NULL, NULL , 0 );
      create_response_packet ( response, status::SIX_WRONG_VERSION );
      send_response_packet ( response );

      return -1;
    }

    // TODO if minor no correct number, return -1
    int minor_int = atoi ( minor );

    if ( minor_int < 0 || minor_int > 255 ) {
      set_packet_body ( response, EMPTY, NULL, NULL , 0 );
      create_response_packet ( response, status::SIX_WRONG_VERSION );
      send_response_packet ( response );

      return -1;
    }

    request->version_major = (uint8_t) major_int;
    request->version_minor = (uint8_t) minor_int;
    

    char output_buffer[150];

    snprintf ( output_buffer, sizeof output_buffer, "parsed_request = { command = '%s', "
        "uuid = '%d', value = '%s', packet_version = '%d.%d' }\r\n",
        INSTRUCTIONS_STRING [ request->command.instruction ],
        //request->command.instruction,
        request->command.uuid,
        request->command.value,
        request->version_major,
        request->version_minor );
    Serial.print ( output_buffer );
   
/*
    if ( *packet_len != 0 ) {
      // TODO sscanf - SEE ABOVE
      // actually it should always be zero - the last string (minor) might be corrupt though
      set_packet_body ( response, EMPTY, NULL, NULL , 0 );
      create_response_packet ( response, status::SIX_SERVER_ERROR );
      send_response_packet ( response );

      return -1;
    }
*/

    return 0;
  }


// ---------------------------------------------------------------------------------------------------------------------
//
//
  int evaluate_command ( request_packet_t* request, response_packet_t* response ) {
    /*
    Serial.println( "\r\nevaluating command..." );
    
    Serial.print ( "request { command = '"  );
    Serial.print ( INSTRUCTIONS_STRING [ request->command.instruction ] );
    Serial.print ( "', uuid = '" );
    Serial.print ( request->command.uuid );
    Serial.print ( "', value = '" );
    Serial.print ( request->command.value );
    Serial.println ( "' }" );
    */

    if ( six::version_major != request->version_major || six::version_minor != request->version_minor ) {
       return -1;
    }

    // ---------------- LIST ACTUATORS --------------------------
    if ( request->command.instruction == six::LIST ) {
      strcpy ( response->body, "" );
      response->body_len = 0;

      for ( uint8_t uuid = 0; uuid < NUMBER_ACTUATORS; uuid++ ) {
        response->body_len += snprintf ( response->body + strlen ( response->body ), 
            REQUEST_RESPONSE_PACKET_LEN - response->body_len, 
            
            "uid: %d\r\n"
            "description: %s\r\n"
            "number elements: %d\r\n"
            "actuator type: %s\r\n"
            "\r\n",
            uuid,
            execute::executor [ uuid ].actuator->description,
            execute::executor [ uuid ].actuator->number_elements,
            actuator::TYPE_STRING [ execute::executor [ uuid ].actuator->type ]
          );
      }

      response->body_len += snprintf ( response->body + strlen ( response->body ), 
          REQUEST_RESPONSE_PACKET_LEN - response->body_len, 
          "\r\n" );

      response->body_len = strlen ( response->body );


      // TODO find the right place for this
      if ( create_response_packet ( response, status::SIX_OK ) != 0 )
        return -1;

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
      else if ( strncasecmp ( "TEMP", request->command.value, request->command.value_len ) == 0 ) {
        mode = execute::TEMPERATURE;
      }
      else if ( strncasecmp ( "SERVO", request->command.value, request->command.value_len ) == 0 ) {
        mode = execute::SERVO;
      }
      else if ( strncasecmp ( "ELEC", request->command.value, request->command.value_len ) == 0 ) {
        mode = execute::SET_ELECTRICAL;
      }
      else if ( strncasecmp ( "OFF", request->command.value, request->command.value_len ) == 0 ) {
        mode = execute::OFF;
      }
      else {
        set_packet_body ( response, EMPTY, NULL, NULL , 0 );
        create_response_packet ( response, status::SIX_NO_SUCH_MODE );
        send_response_packet ( response );

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
    Serial.println ( "sending response..." );

    char response_buf [ REQUEST_RESPONSE_PACKET_LEN ];

    snprintf ( response_buf, REQUEST_RESPONSE_PACKET_LEN,
        "%d %s SIX/%d.%d\r\n"
        "%s\r\n\r\n",
        packet->status.CODE, packet->status.DESCRIPTION, packet->version_major, packet->version_minor,
        packet->body );

    /*
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
    */

    Serial.println ( response_buf );
    // TODO check if cast is correct!
    ble_write_bytes ( (unsigned char*)response_buf, strlen ( response_buf ) );

    return 0;
  }



// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
  
  // returns new position of inside the packet after the space
  char* parse_next ( char* packet_segment, size_t* packet_len, size_t* segment_len, char* segment_type  ) {

    //TODO use memchr instead!
    char* space = (char*) memchr ( (void*) packet_segment, ' ', *packet_len );
    //char* space = strchr ( packet_segment, ' ' );
   
    if ( space == NULL ) {
      return NULL;
    }

    else {
     *segment_len = space - packet_segment;
    }

    if ( *segment_len < 1 ) {
      return NULL;
    }

    *space = '\0';

    return space + 1;
  }

  int create_response_packet ( response_packet_t* packet, status::status_type status ) {

    packet->version_major = six::version_major;
    packet->version_minor = six::version_minor;

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
