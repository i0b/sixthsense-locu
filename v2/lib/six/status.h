#ifndef STATUS_H
#define STATUS_H

#include <stddef.h>
#include <stdint.h>

namespace six {
  
  typedef enum {
    SIX_OK,
    SIX_COMMAND_NOT_FOUND,
    SIX_NO_SUCH_MODE,
    SIX_NO_SUCH_ACTUATOR,
    SIX_SERVER_ERROR,
    SIX_ERROR_PARSING,
    SIX_WRONG_VERSION
  } statusType;

  typedef struct {
    statusType status;
    uint16_t code;
    char* description;
  } status_t;


  extern status_t statusDescription[];

}

#endif
