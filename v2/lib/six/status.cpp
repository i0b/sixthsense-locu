#include "status.h"

namespace six {

  status_t statusDescription[] = {
    {
      SIX_OK,
      200,
      "OK"
    },
    {
      SIX_COMMAND_NOT_FOUND,
      404,
      "ERROR command not found"
    },
    {
      SIX_NO_SUCH_MODE,
      405,
      "ERROR no such mode"
    },
    {
      SIX_NO_SUCH_ACTUATOR,
      406,
      "ERROR actuator id not found"
    },
    {
      SIX_SERVER_ERROR,
      500,
      "ERROR wrong command applied"
    },
    {
      SIX_ERROR_PARSING,
      503,
      "ERROR command not recognized"
    },
    {
      SIX_WRONG_VERSION,
      505,
      "ERROR the protocol version you are using is incompatible"
    }

  };

}
