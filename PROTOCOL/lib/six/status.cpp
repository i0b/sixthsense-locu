#include "status.h"

namespace status {
/*
  enum state {
    SIX_OK,
    SIX_NOT_FOUND;
    SIX_ERROR_PARSING
  } state_t;
*/

  status_t status_description[] = {
    {
      SIX_OK,
      200,
      "OK"
    },
    {
      SIX_NOT_FOUND,
      404,
      "ERROR command not found"
    },
    {
      SIX_ERROR_PARSING,
      503,
      "ERROR command not recognized"
    }
  };

}
