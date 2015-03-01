#include "executor.h"
#include "status.h"
#include "type.h"
#include "parser.h"

#include <Arduino.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <SPI.h>

namespace six {

  Parser::Parser(Executor* executor) {
    _executor = executor;
    _packetLength = 0;
  }

  void Parser::reset() {
    _packetLength = 0;
  }

  void Parser::append(char c) {
    size_t oldLength = _packetLength;
    _packetLength = (_packetLength+1)%REQUEST_RESPONSE_PACKET_LENGTH;
    _packetData[oldLength] = c;
    _packetData[_packetLength] = '\0';
  }

  int Parser::parseCommand(){
    return Parser::parseCommand(_packetData, &_packetLength);
  }

  int Parser::parseCommand(char* rawPacket, size_t* packetLength) {

    // parse command - example GETV
    char* command = rawPacket;
    size_t commandLength;

    rawPacket = _parseNext(command, packetLength, &commandLength, "command");

    if (rawPacket == NULL) {
      _setPacketBody(EMPTY, NULL, NULL , 0);
      _createResponsePacket(six::SIX_ERROR_PARSING);
      _sendResponsePacket();

      return -1;
    }
     
    *packetLength -= commandLength;

    // evaluate command string

    if (strncasecmp ("LIST", command, commandLength) == 0) {
      _requestPacket.command.instruction = instructions::LIST;
    }

    else if (strncasecmp ("PING", command, commandLength) == 0) {
      //Serial.println("DEBUG: parsed 'ping' as valid command");
      _requestPacket.command.instruction = instructions::PING_DEVICE;
    }

    else if (strncasecmp ("DISCDEMO", command, commandLength) == 0) {
      _requestPacket.command.instruction = instructions::DEMONSTRATE_DISCONNECT;
    }

    else {
      char* id = rawPacket;
      size_t idLength;

      // read id
      rawPacket = _parseNext (id, packetLength, &idLength, "id");

      if (rawPacket == NULL) {
        _setPacketBody(EMPTY, NULL, NULL , 0);
        _createResponsePacket(six::SIX_ERROR_PARSING);
        _sendResponsePacket();

        return -1;
      }
    
      *packetLength -= idLength;

      int idIntegerValue = atoi (id);

      if (idIntegerValue < 0 || idIntegerValue > 255) {
        _setPacketBody(EMPTY, NULL, NULL , 0);
        // TODO error MALFORMAT
        _createResponsePacket(six::SIX_ERROR_PARSING);
        _sendResponsePacket();

        return -1;
      }

      _requestPacket.command.id = (uint8_t) idIntegerValue;

      if (strncasecmp ("GM", command, commandLength) == 0) {
        _requestPacket.command.instruction = instructions::GET_MODE;
      }
      else if (strncasecmp ("GI", command, commandLength) == 0) {
        _requestPacket.command.instruction = instructions::GET_INTENSITY;
      }
      else if (strncasecmp ("GP", command, commandLength) == 0) {
        _requestPacket.command.instruction = instructions::GET_PARAMETER;
      }

      else
      {
        char* value = rawPacket;
        size_t valueLength;

        // read value
        rawPacket = _parseNext (value, packetLength, &valueLength, "value");
      
        if (rawPacket == NULL) {
          _setPacketBody(EMPTY, NULL, NULL , 0);
          _createResponsePacket(six::SIX_ERROR_PARSING);
          _sendResponsePacket();

          return -1;
        }
      
        *packetLength -= valueLength;

        _requestPacket.command.value = value;
        _requestPacket.command.valueLength = valueLength;

        if (strncasecmp ("SM", command, commandLength) == 0) {
          _requestPacket.command.instruction = instructions::SET_MODE;
        }
        else if (strncasecmp ("SI", command, commandLength) == 0) {
          _requestPacket.command.instruction = instructions::SET_INTENSITY;
        }
        else if (strncasecmp ("SP", command, commandLength) == 0) {
          _requestPacket.command.instruction = instructions::SET_PARAMETER;
        }
        else {
          _setPacketBody(EMPTY, NULL, NULL , 0);
          _createResponsePacket(six::SIX_COMMAND_NOT_FOUND);
          _sendResponsePacket();

          return -1;
        }
      }
    }
    /*

    // compare version - example: SIX/0.1
    char* protocolName = "SIX/";

    if (strncasecmp (protocolName, rawPacket, strlen (protocolName)) == 0) {
      rawPacket += strlen (protocolName);
    }
    else {
      _setPacketBody(EMPTY, NULL, NULL , 0);
      _createResponsePacket(six::SIX_ERROR_PARSING);
      _sendResponsePacket();

      return -1;
    }
    
    *packetLength -= strlen (protocolName);

    char* dot = (char*) memchr ((void*)rawPacket, '.', *packetLength);

    if (dot == NULL) {
      _setPacketBody(EMPTY, NULL, NULL , 0);
      _createResponsePacket(six::SIX_ERROR_PARSING);
      _sendResponsePacket();

      return -1;
    }

    *dot = '\0';
    
    char* major = rawPacket;
    size_t majorLength = strlen (major);

    // TODO this still allows: LIST SIX/0.1somethingelse
    // USE SSCANF (%d) and check if string is empty afterwards!
    char* minor = dot + 1;
    size_t minorLength = strlen (minor);
    

    *packetLength -= majorLength + minorLength + 1; // 1 = '.'

    int majorInt = atoi (major);

    if (majorInt < 0 || majorInt > 255) {
      _setPacketBody(EMPTY, NULL, NULL , 0);
      _createResponsePacket(six::SIX_WRONG_VERSION);
      _sendResponsePacket();

      return -1;
    }

    // TODO if minor no correct number, return -1
    int minorInt = atoi (minor);

    if (minorInt < 0 || minorInt > 255) {
      _setPacketBody(EMPTY, NULL, NULL , 0);
      _createResponsePacket(six::SIX_WRONG_VERSION);
      _sendResponsePacket();

      return -1;
    }
    */

    _requestPacket.versionMajor = VERSION_MAJOR;//(uint8_t) majorInt;
    _requestPacket.versionMinor = VERSION_MINOR;//(uint8_t) minorInt;

    char outputBuffer[256];

    snprintf (outputBuffer, sizeof outputBuffer, "DEBUG: parsedRequest = { command = '', "
        "id = '%d', value = '%s', packetVersion = '%d.%d' }\r\n",
        //INSTRUCTIONS_STRING [ _requestPacket.command.instruction ],
        //_requestPacket.command.instruction,
        _requestPacket.command.id,
        _requestPacket.command.value,
        _requestPacket.versionMajor,
        _requestPacket.versionMinor);
    Serial.print (outputBuffer);
   
/*
    if (*packetLength != 0) {
      // TODO sscanf - SEE ABOVE
      // actually it should always be zero - the last string (minor) might be corrupt though
      _setPacketBody(EMPTY, NULL, NULL , 0);
      _createResponsePacket(six::SIX_SERVER_ERROR);
      _sendResponsePacket();

      return -1;
    }
*/

    return 0;
  }


// ---------------------------------------------------------------------------------------------------------------------
//
//
  int Parser::evaluateCommand() {

    if (_requestPacket.versionMajor != VERSION_MAJOR|| _requestPacket.versionMinor != VERSION_MINOR ) {
       return -1;
    }

    // ---------------- LIST ACTUATORS --------------------------
    //
    if (_requestPacket.command.instruction == instructions::LIST) {
      strcpy (_responsePacket.body, "");
      _responsePacket.bodyLength = 0;

      _executor->list(&_responsePacket.body, &_responsePacket.bodyLength);

      _responsePacket.bodyLength += snprintf (_responsePacket.body + strlen (_responsePacket.body), 
          REQUEST_RESPONSE_PACKET_LENGTH - _responsePacket.bodyLength, 
          "\r\n");

      _responsePacket.bodyLength = strlen (_responsePacket.body);


      // TODO find the right place for this
      if (_createResponsePacket(six::SIX_OK) != 0)
        return -1;

      // TODO find the right place for this
      _sendResponsePacket();

      return 0;
    }

    // ------------------ KEEPALIVE PING ------------------------
    //
    else if (_requestPacket.command.instruction == instructions::PING_DEVICE) {
      _executor->ping();

      _setPacketBody(EMPTY, NULL, NULL , 0);
      _createResponsePacket(six::SIX_OK);
      _sendResponsePacket();

      return 0;
    }

    // ----------- DEMONSTRATE DISCONNECT PATTERN ---------------
    //
    else if (_requestPacket.command.instruction == instructions::DEMONSTRATE_DISCONNECT) {
      _setPacketBody(EMPTY, NULL, NULL , 0);
      _createResponsePacket(six::SIX_OK);
      _sendResponsePacket();

      _executor->demonstrateDisconnect();

      return 0;
    }

    // ------------------ GET MODE ------------------------------
    //
    else if (_requestPacket.command.instruction == instructions::GET_MODE) {
      
      //TODO FIXME
      /*
      if (_setPacketBody(STRING, "mode", executionModeString[_executor->getMode(_requestPacket.command.id)]) == 0) {
        _createResponsePacket(six::SIX_OK);
        _sendResponsePacket();
      }
      */

      return 0;
    }

    // ---------------- GET INTENSITY ---------------------------
    //
    else if (_requestPacket.command.instruction == instructions::GET_INTENSITY) {

      if (_setPacketBody(INT, "intensity", NULL, _executor->getIntensity(_requestPacket.command.id)) == 0) {
        _createResponsePacket(six::SIX_OK);
        _sendResponsePacket();
      }

      return 0;
    }

    // TODO NOT COPY-PASTE!!
    // ---------------- GET PARAMETER ---------------------------
    //
    else if (_requestPacket.command.instruction == instructions::GET_PARAMETER) {
      
      if (_setPacketBody(INT, "parameter", NULL, _executor->getParameter(_requestPacket.command.id)) == 0) {
        _createResponsePacket(six::SIX_OK);
        _sendResponsePacket();
      }

      return 0;
    }


    // -------------------- SET MODE ----------------------------
    //
    else if (_requestPacket.command.instruction == instructions::SET_MODE) {
      executionMode mode = executionMode::OFF;

      if (strncasecmp ("BEAT", _requestPacket.command.value, _requestPacket.command.valueLength) == 0) {
        mode = executionMode::HEARTBEAT;
      }
      else if (strncasecmp ("ROT", _requestPacket.command.value, _requestPacket.command.valueLength) == 0) {
        mode = executionMode::ROTATION;
      }
      else if (strncasecmp ("VIB", _requestPacket.command.value, _requestPacket.command.valueLength) == 0) {
        mode = executionMode::VIBRATION;
      }
      else if (strncasecmp ("TEMP", _requestPacket.command.value, _requestPacket.command.valueLength) == 0) {
        mode = executionMode::TEMPERATURE;
      }
      else if (strncasecmp ("PRESS", _requestPacket.command.value, _requestPacket.command.valueLength) == 0) {
        mode = executionMode::PRESSURE;
      }
      else if (strncasecmp ("ELEC", _requestPacket.command.value, _requestPacket.command.valueLength) == 0) {
        mode = executionMode::ELECTRO;
      }
      else if (strncasecmp ("OFF", _requestPacket.command.value, _requestPacket.command.valueLength) == 0) {
        mode = executionMode::OFF;
      }
      else {
        _setPacketBody(EMPTY, NULL, NULL , 0);
        _createResponsePacket(six::SIX_NO_SUCH_MODE);
        _sendResponsePacket();

        return -1;
      }
      
      _executor->setMode(_requestPacket.command.id, mode);


      _setPacketBody(EMPTY, NULL, NULL , 0);
      _createResponsePacket(six::SIX_OK);
      _sendResponsePacket();

      return 0;
    }
    
    // -------------------- SET INTENSITY -----------------------
    //
    else if (_requestPacket.command.instruction == instructions::SET_INTENSITY) {
      int intensity = atoi(_requestPacket.command.value);
      //TODO check if parameter valide
      _executor->setIntensity(_requestPacket.command.id, intensity);

      _setPacketBody(EMPTY, NULL, NULL , 0);
      _createResponsePacket(six::SIX_OK);
      _sendResponsePacket();

      return 0;
    }

    // -------------------- SET PARAMETER -----------------------
    //
    else if (_requestPacket.command.instruction == instructions::SET_PARAMETER) {
      int parameter = atoi(_requestPacket.command.value);
      //TODO check if parameter valide
      _executor->setParameter(_requestPacket.command.id, parameter);

      _setPacketBody(EMPTY, NULL, NULL , 0);
      _createResponsePacket(six::SIX_OK);
      _sendResponsePacket();

      return 0;
    }
  }


// ---------------------------------------------------------------------------------------------------------------------
//
//
  int Parser::_sendResponsePacket() {

    //Serial.println ("sending response...");
    char responseBuf [ REQUEST_RESPONSE_PACKET_LENGTH ];

    snprintf (responseBuf, REQUEST_RESPONSE_PACKET_LENGTH,
        "%d %s SIX/%d.%d\r\n"
        "%s\r\n\r\n",
        _responsePacket.status.code, _responsePacket.status.description, _responsePacket.versionMajor, _responsePacket.versionMinor,
        _responsePacket.body);

    /*
    Serial.print (packet->status.CODE);
    Serial.print (" ");
    Serial.print (packet->status.DESCRIPTION);
    Serial.print (" ");
    Serial.print ("SIX/");
    Serial.print (packet->versionMajor);
    Serial.print (".");
    Serial.print (packet->versionMinor);
    Serial.print ("\r\n");
    Serial.print ("\r\n");

    Serial.print (packet->body);
    Serial.print ("\r\n");


    Serial.print ("\r\n");
    Serial.print ("\r\n");
    */

    Serial.println (responseBuf);
    BLEMini.println (responseBuf);

    return 0;
  }



// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
  
  // returns new position of inside the packet after the space
  char* Parser::_parseNext(char* packetSegment, size_t* packetLength, size_t* segmentLength, char* segmentType) {

    //TODO use memchr instead!
    char* space = (char*) memchr ((void*) packetSegment, ' ', *packetLength);
    //char* space = strchr (packetSegment, ' ');
   
    if (space == NULL) {
      return NULL;
    }

    else {
     *segmentLength = space - packetSegment + 1;
    }

    if (*segmentLength < 1) {
      return NULL;
    }

    *space = '\0';
    //Serial.println(packetSegment);
    //char tmp [100];
    //snprintf (tmp, strlen(packetSegment)+10, "%s: %d --\r\n", packetSegment, *segmentLength);

    return space + 1;
  }

// ---------------------------------------------------------------------------------------------------------------------

  int Parser::_createResponsePacket(six::statusType status) {
    _responsePacket.versionMajor = VERSION_MAJOR;
    _responsePacket.versionMinor = VERSION_MINOR;

    _responsePacket.status = six::statusDescription [ status ];

    // TODO find the right place for this
    //if (packet->status.status != six::SIX_OK) {
    //  return -1;
    //}

    return 0;
  }

// ---------------------------------------------------------------------------------------------------------------------

  int Parser::_setPacketBody(_value_t type, const char* valueInformation, const char* charValue, int intValue) {

    strcpy(_responsePacket.body, "");
    _responsePacket.bodyLength = 0;
    
    if (type == EMPTY) {
      return 0;
    }

    else if (type == INT) {
      _responsePacket.bodyLength += snprintf (
          _responsePacket.body + strlen (_responsePacket.body),
          REQUEST_RESPONSE_PACKET_LENGTH - _responsePacket.bodyLength,
          "%s: %d\r\n\r\n",
          valueInformation,
          intValue
       );
    }
    
    else if (type == STRING) {
      _responsePacket.bodyLength += snprintf (
          _responsePacket.body + strlen (_responsePacket.body),
          REQUEST_RESPONSE_PACKET_LENGTH - _responsePacket.bodyLength,
          "%s: %s\r\n\r\n",
          valueInformation,
          charValue
       );
    }

    _responsePacket.bodyLength = strlen (_responsePacket.body);

    return 0;
  }

}
