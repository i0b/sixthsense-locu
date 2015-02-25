#include "six.h"
#include "actuator.h"
#include "execute.h"
#include "status.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <SPI.h>

#ifndef GENERATE
#define GERERATE

#endif

    public:
      // upon successful completion, these functions shall return 0. Otherwise, these functions shall return −1
      static int parseCommand ( char* rawPacket, size_t packetLength );
      static int evaluateCommand ();
      static int sendResponsePacket ();
    private:
      static char _packetData [ requestResponsePacketLength ];
      static size_t _packetLength;
      static requestPacket_t _requestPacket;
      static responsePacket_t _responsePacket;

      static char* _parseNext ( char* packetSegment, size_t* packetLength, size_t* segmentLength, char* segmentType  );
      static int _setPacketBody ( responsePacket_t* packet, value_t type, const char* valueInformation, const char* charValue, int intValue );
      static int _createResponsePacket ( responsePacket_t* packet, status::statusType status );




namespace parse {
  typedef enum { EMPTY, INT, STRING } value_t;
  static const char* INSTRUCTIONS_STRING[] = { FOREACH_INSTRUCTION ( GENERATE_STRING ) };

  static int Parse::parseCommand ( char* rawPacket, size_t packetLength ) {

    // parse command - example GETV
    char* command = rawPacket;
    size_t commandLength;

    rawPacket = _parseNext ( command, packetLength, &commandLength, "command" );

    if ( rawPacket == NULL ) {
      _setPacketBody ( EMPTY, NULL, NULL , 0 );
      _createResponsePacket ( status::SIX_ERROR_PARSING );
      _sendResponsePacket ();

      return -1;
    }
     
    *packetLength -= commandLength;

    // evaluate command string

    if ( strncasecmp ( "LIST", command, commandLength ) == 0 ) {
      _requestPacket->command.instruction = six::LIST;
    }

    else if ( strncasecmp ( "PING", command, commandLength ) == 0 ) {
      //Serial.println("DEBUG: parsed 'ping' as valid command");
      _requestPacket->command.instruction = six::PING_DEVICE;
    }

    else if ( strncasecmp ( "DISCDEMO", command, commandLength ) == 0 ) {
      _requestPacket->command.instruction = six::DEMONSTRATE_DISCONNECT;
    }

    else {
      char* uid = rawPacket;
      size_t uidLength;

      // read uuid
      rawPacket = _parseNext ( uid, packetLength, &uidLength, "uuid" );

      if ( rawPacket == NULL ) {
        _setPacketBody ( EMPTY, NULL, NULL , 0 );
        _createResponsePacket ( status::SIX_ERROR_PARSING );
        _sendResponsePacket ();

        return -1;
      }
    
      *packetLength -= uidLength;

      int uidIntegerValue = atoi ( uid );

      if ( uidIntegerValue < 0 || uidIntegerValue > 255 ) {
        _setPacketBody ( EMPTY, NULL, NULL , 0 );
        // TODO error MALFORMAT
        _createResponsePacket ( status::SIX_ERROR_PARSING );
        _sendResponsePacket ();

        return -1;
      }

      _requestPacket->command.uid = (uint8_t) uidIntegerValue;

      if ( strncasecmp ( "GM", command, commandLength ) == 0 ) {
        _requestPacket->command.instruction = six::GET_MODE;
      }
      else if ( strncasecmp ( "GI", command, commandLength ) == 0 ) {
        _requestPacket->command.instruction = six::GET_INTENSITY;
      }
      else if ( strncasecmp ( "GP", command, commandLength ) == 0 ) {
        _requestPacket->command.instruction = six::GET_PARAMETER;
      }

      else
      {
        char* value = rawPacket;
        size_t valueLength;

        // read value
        rawPacket = _parseNext ( value, packetLength, &valueLength, "value" );
      
        if ( rawPacket == NULL ) {
          _setPacketBody ( EMPTY, NULL, NULL , 0 );
          _createResponsePacket ( status::SIX_ERROR_PARSING );
          _sendResponsePacket ();

          return -1;
        }
      
        *packetLength -= valueLength;

        _requestPacket->command.value = value;
        _requestPacket->command.valueLength = valueLength;

        if ( strncasecmp ( "SM", command, commandLength ) == 0 ) {
          _requestPacket->command.instruction = six::SET_MODE;
        }
        else if ( strncasecmp ( "SI", command, commandLength ) == 0 ) {
          _requestPacket->command.instruction = six::SET_INTENSITY;
        }
        else if ( strncasecmp ( "SP", command, commandLength ) == 0 ) {
          _requestPacket->command.instruction = six::SET_PARAMETER;
        }
        else {
          _setPacketBody ( EMPTY, NULL, NULL , 0 );
          _createResponsePacket ( status::SIX_COMMAND_NOT_FOUND );
          _sendResponsePacket ();

          return -1;
        }
      }
    }
    /*

    // compare version - example: SIX/0.1
    char* protocolName = "SIX/";

    if ( strncasecmp ( protocolName, rawPacket, strlen ( protocolName ) ) == 0 ) {
      rawPacket += strlen ( protocolName );
    }
    else {
      _setPacketBody ( EMPTY, NULL, NULL , 0 );
      _createResponsePacket ( status::SIX_ERROR_PARSING );
      _sendResponsePacket ();

      return -1;
    }
    
    *packetLength -= strlen ( protocolName );

    char* dot = (char*) memchr ( (void*)rawPacket, '.', *packetLength );

    if ( dot == NULL ) {
      _setPacketBody ( EMPTY, NULL, NULL , 0 );
      _createResponsePacket ( status::SIX_ERROR_PARSING );
      _sendResponsePacket ();

      return -1;
    }

    *dot = '\0';
    
    char* major = rawPacket;
    size_t majorLength = strlen ( major );

    // TODO this still allows: LIST SIX/0.1somethingelse
    // USE SSCANF ( %d ) and check if string is empty afterwards!
    char* minor = dot + 1;
    size_t minorLength = strlen ( minor );
    

    *packetLength -= majorLength + minorLength + 1; // 1 = '.'

    int major_int = atoi ( major );

    if ( major_int < 0 || major_int > 255 ) {
      _setPacketBody ( EMPTY, NULL, NULL , 0 );
      _createResponsePacket ( status::SIX_WRONG_VERSION );
      _sendResponsePacket ();

      return -1;
    }

    // TODO if minor no correct number, return -1
    int minor_int = atoi ( minor );

    if ( minor_int < 0 || minor_int > 255 ) {
      _setPacketBody ( EMPTY, NULL, NULL , 0 );
      _createResponsePacket ( status::SIX_WRONG_VERSION );
      _sendResponsePacket ();

      return -1;
    }
    */

    _requestPacket->versionMajor = versionMajor;//(uint8_t) major_int;
    _requestPacket->versionMinor = versionMinor;//(uint8_t) minor_int;

    char outputBuffer[256];

    snprintf ( outputBuffer, sizeof outputBuffer, "DEBUG: parsed_request = { command = '%s', "
        "uuid = '%d', value = '%s', packet_version = '%d.%d' }\r\n",
        INSTRUCTIONS_STRING [ _requestPacket->command.instruction ],
        //_requestPacket->command.instruction,
        _requestPacket->command.uid,
        _requestPacket->command.value,
        _requestPacket->versionMajor,
        _requestPacket->versionMinor );
    Serial.print ( outputBuffer );
   
/*
    if ( *packetLength != 0 ) {
      // TODO sscanf - SEE ABOVE
      // actually it should always be zero - the last string (minor) might be corrupt though
      _setPacketBody ( EMPTY, NULL, NULL , 0 );
      _createResponsePacket ( status::SIX_SERVER_ERROR );
      _sendResponsePacket ();

      return -1;
    }
*/

    return 0;
  }


// ---------------------------------------------------------------------------------------------------------------------
//
//
  int _evaluateCommand () {

    if ( six::versionMajor != _requestPacket->versionMajor || six::versionMinor != _requestPacket->versionMinor ) {
       return -1;
    }

    // ---------------- LIST ACTUATORS --------------------------
    //
    if ( _requestPacket->command.instruction == six::LIST ) {
      strcpy ( _responsePacket->body, "" );
      _responsePacket->bodyLength = 0;

      for ( uint8_t uid = 0; uid < six::Six::_numberActuators; uid++ ) {
        _responsePacket->bodyLength += snprintf ( _responsePacket->body + strlen ( _responsePacket->body ), 
            REQUEST_RESPONSE_PACKET_LEN - _responsePacket->bodyLength, 
            
            "uid: %d\r\n"
            "description: %s\r\n"
            "number elements: %d\r\n"
            "actuator type: %s\r\n"
            "\r\n",
            uuid,
            actuator::actuators [ uuid ].description,
            actuator::actuators [ uuid ].number_elements,
            actuator::TYPE_STRING [ actuator::actuators [ uuid ].type ]
          );
      }

      _responsePacket->bodyLength += snprintf ( _responsePacket->body + strlen ( _responsePacket->body ), 
          REQUEST_RESPONSE_PACKET_LEN - _responsePacket->bodyLength, 
          "\r\n" );

      _responsePacket->bodyLength = strlen ( _responsePacket->body );


      // TODO find the right place for this
      if ( _createResponsePacket ( status::SIX_OK ) != 0 )
        return -1;

      // TODO find the right place for this
      _sendResponsePacket ();

      return 0;
    }

    // ------------------ KEEPALIVE PING ------------------------
    //
    else if ( _requestPacket->command.instruction == six::PING_DEVICE ) {
      //Serial.println("DEBUG: call execute::ping(), create SIX_OK response");

      execute::ping ();

      _setPacketBody ( EMPTY, NULL, NULL , 0 );
      _createResponsePacket ( status::SIX_OK );
      _sendResponsePacket ();

      return 0;
    }

    // ----------- DEMONSTRATE DISCONNECT PATTERN ---------------
    //
    else if ( _requestPacket->command.instruction == six::DEMONSTRATE_DISCONNECT ) {
      _setPacketBody ( EMPTY, NULL, NULL , 0 );
      _createResponsePacket ( status::SIX_OK );
      _sendResponsePacket ();

      execute::demonstrate_disconnect ();

      return 0;
    }

    // ------------------ GET MODE ------------------------------
    //
    else if ( _requestPacket->command.instruction == six::GET_MODE ) {
      
      if ( _setPacketBody ( STRING, "mode", 
           execute::EXECUTION_MODE_STRING [ actuator::actuators [ _requestPacket->command.uuid ].mode ], 0 ) == 0 ) {

        _createResponsePacket ( status::SIX_OK );

        _sendResponsePacket ();
      }

      return 0;
    }

    // ---------------- GET INTENSITY ---------------------------
    //
    else if ( _requestPacket->command.instruction == six::GET_INTENSITY ) {

      if ( _setPacketBody ( INT, "intensity", NULL, 
            actuator::actuators [ _requestPacket->command.uuid ].intensity ) == 0 ) {

        _createResponsePacket ( status::SIX_OK );

        _sendResponsePacket ();
      }

      return 0;
    }

    // TODO NOT COPY-PASTE!!
    // ---------------- GET PARAMETER ---------------------------
    //
    else if ( _requestPacket->command.instruction == six::GET_PARAMETER ) {
      
      if ( _setPacketBody ( INT, "parameter", NULL, 
            actuator::actuators [ _requestPacket->command.uuid ].parameter ) == 0 ) {

        _createResponsePacket ( status::SIX_OK );

        _sendResponsePacket ();
      }

      return 0;
    }


    // -------------------- SET MODE ----------------------------
    //
    else if ( _requestPacket->command.instruction == six::SET_MODE ) {
      execute::execution_mode mode = execute::OFF;

      if ( strncasecmp ( "BEAT", _requestPacket->command.value, _requestPacket->command.valueLength ) == 0 ) {
        mode = execute::HEARTBEAT;
      }
      else if ( strncasecmp ( "ROT", _requestPacket->command.value, _requestPacket->command.valueLength ) == 0 ) {
        mode = execute::ROTATION;
      }
      else if ( strncasecmp ( "VIB", _requestPacket->command.value, _requestPacket->command.valueLength ) == 0 ) {
        mode = execute::VIBRATION;
      }
      else if ( strncasecmp ( "TEMP", _requestPacket->command.value, _requestPacket->command.valueLength ) == 0 ) {
        mode = execute::TEMPERATURE;
      }
      else if ( strncasecmp ( "SERVO", _requestPacket->command.value, _requestPacket->command.valueLength ) == 0 ) {
        mode = execute::SERVO;
      }
      else if ( strncasecmp ( "ELEC", _requestPacket->command.value, _requestPacket->command.valueLength ) == 0 ) {
        mode = execute::SET_ELECTRO;
      }
      else if ( strncasecmp ( "OFF", _requestPacket->command.value, _requestPacket->command.valueLength ) == 0 ) {
        mode = execute::OFF;
      }
      else {
        _setPacketBody ( EMPTY, NULL, NULL , 0 );
        _createResponsePacket ( status::SIX_NO_SUCH_MODE );
        _sendResponsePacket ();

        return -1;
      }
      
      execute::set_mode ( _requestPacket->command.uuid, mode );


      _setPacketBody ( EMPTY, NULL, NULL , 0 );
      _createResponsePacket ( status::SIX_OK );
      _sendResponsePacket ();

      return 0;
    }
    
    // -------------------- SET INTENSITY -----------------------
    //
    else if ( _requestPacket->command.instruction == six::SET_INTENSITY ) {
      int intensity = atoi ( _requestPacket->command.value );
      //TODO check if parameter valide
      execute::set_intensity ( _requestPacket->command.uuid, intensity );

      _setPacketBody ( EMPTY, NULL, NULL , 0 );
      _createResponsePacket ( status::SIX_OK );
      _sendResponsePacket ();

      return 0;
    }

    // -------------------- SET PARAMETER -----------------------
    //
    else if ( _requestPacket->command.instruction == six::SET_PARAMETER ) {
      int parameter = atoi ( _requestPacket->command.value );
      //TODO check if parameter valide
      execute::set_parameter ( _requestPacket->command.uuid, parameter );

      _setPacketBody ( EMPTY, NULL, NULL , 0 );
      _createResponsePacket ( status::SIX_OK );
      _sendResponsePacket ();

      return 0;
    }
  }


// ---------------------------------------------------------------------------------------------------------------------
//
//
  int send_response_packet ( response_packet_t* packet ) {

    //Serial.println ( "sending response..." );
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
    BLEMini.println ( response_buf );

    return 0;
  }



// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
  
  // returns new position of inside the packet after the space
  char* parse_next ( char* packet_segment, size_t* packetLength, size_t* segmentLength, char* segment_type  ) {

    //TODO use memchr instead!
    char* space = (char*) memchr ( (void*) packet_segment, ' ', *packetLength );
    //char* space = strchr ( packet_segment, ' ' );
   
    if ( space == NULL ) {
      return NULL;
    }

    else {
     *segmentLength = space - packet_segment + 1;
    }

    if ( *segmentLength < 1 ) {
      return NULL;
    }

    *space = '\0';
    //Serial.println(packet_segment);
    //char tmp [100];
    //snprintf ( tmp, strlen(packet_segment)+10, "%s: %d --\r\n", packet_segment, *segmentLength);

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
    packet->bodyLength = 0;
    
    if ( type == EMPTY ) {
      return 0;
    }

    else if ( type == INT ) {
      packet->bodyLength += snprintf (
          packet->body + strlen ( packet->body ),
          REQUEST_RESPONSE_PACKET_LEN - packet->bodyLength,
          "%s: %d\r\n\r\n",
          value_information,
          int_value
        );
    }
    
    else if ( type == STRING ) {
      packet->bodyLength += snprintf (
          packet->body + strlen ( packet->body ),
          REQUEST_RESPONSE_PACKET_LEN - packet->bodyLength,
          "%s: %s\r\n\r\n",
          value_information,
          char_value
        );
    }

    packet->bodyLength = strlen ( packet->body );

    return 0;
  }

}
