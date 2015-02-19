#ifndef ENABLED_ACTUATORS_HPP
#define ENABLED_ACTUATORS_HPP



//#define NUMBER_ACTUATORS 2
//see: lib/six/actuator.h


actuator_t actuators [ NUMBER_ACTUATORS ] = {

  #include "vibration_armband_left.hpp"
//#include "servo_armband.hpp"
  #include "peltier_armband_left.hpp"
//#include "electro_controller.hpp"

};

#endif
