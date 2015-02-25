#ifndef STATUS_H
#define STATUS_H

#include <stddef.h>
#include <stdint.h>


namespace status {
  
  typedef enum {
    SIX_OK,
    SIX_COMMAND_NOT_FOUND,
    SIX_NO_SUCH_MODE,
    SIX_SERVER_ERROR,
    SIX_ERROR_PARSING,
    SIX_WRONG_VERSION
  } status_type;

  typedef struct {
    status_type status;
    uint16_t CODE;
    char* DESCRIPTION;
  } status_t;


  extern status_t status_description[];

}

#endif
