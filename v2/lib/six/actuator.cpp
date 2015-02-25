#include "actuator.h"

namespace actuator {
  
  const char* TYPE_STRING[] = { FOREACH_TYPE ( GENERATE_STRING ) };

  #include "actuators/enabled_actuators.hpp"

  // calculate the total number of actuators
  // const uint8_t NUMBER_ACTUATORS = sizeof ( actuators ) / sizeof ( actuator_t );

}
