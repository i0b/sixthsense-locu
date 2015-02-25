#include "actuator.h"

namespace actuator {  
  static const char* ACTUATOR_TYPE_STRING[] = { FOREACH_TYPE ( GENERATE_STRING ) };
}
