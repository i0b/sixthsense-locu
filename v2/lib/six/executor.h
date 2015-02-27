#ifndef EXECUTER_H 
#define EXECUTER_H 

#include "six.h"
#include "actuator.h"
#include "adafruit.h"
#include <stddef.h>
#include <stdint.h>
#include <QueueList.h>

namespace six {

  class Executor {
    public:
      Executor(Actuator* actuator);

      int runExecutor();
      int list(char** body, size_t* bodyLength);
      int ping();
      int demonstrateDisconnect();

      int setMode(uint8_t id, Six::executionMode mode);
      int setIntensity(uint8_t id, int intensity);
      int setParameter(uint8_t id, int parameter);
      Six::executionMode getMode(uint8_t id);
      int getIntensity(uint8_t id);
      int getParameter(uint8_t id);

      static void (six::Executor::*executorIsr)();

    
    private:
      Actuator* _actuator;
      Adafruit* _adafruit;

      typedef struct {
        uint8_t id;
        int intensity;
        int parameter;
        void (six::Executor::*function)(uint8_t id, int intensity, int parameter);
      } _execution_t;

      // executeable functions
      void _executeDisconnect(uint8_t id, int intensity, int parameter);
      void _executeOff(uint8_t id, int intensity, int parameter);
      void _executeServo(uint8_t id, int intensity, int parameter);
      void _electroStimulation(uint8_t id, int intensity, int parameter);
      void _executeTemperature(uint8_t id, int intensity, int parameter);
      void _executeVibration(uint8_t id, int intensity, int parameter);

      uint32_t _executionTimer;
      uint16_t _keepAliveTimer;
      uint8_t  _connected;

      uint16_t _keepAliveTimeout = 2000; // 200 * 10 ms = 2 sec

      QueueList <_execution_t> _executionQueue;
      void _timerIsr();

  };

  ISR(TIMER3_COMPA_vect) {
    //*(Executor::executorIsr)();
  }


}

#endif
