#ifndef STATUS_H
#define STATUS_H

#include <stddef.h>
#include <stdint.h>


namespace status {
  
  typedef enum {
    SIX_OK,
    SIX_NOT_FOUND,
    SIX_SERVER_ERROR,
    SIX_ERROR_PARSING
  } status_type;

  typedef struct {
    status_type status;
    uint16_t CODE;
    char* DESCRIPTION;
  } status_t;


  extern status_t status_description[];

}

#endif
