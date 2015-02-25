#include "execute.h"
#include "actuator.h"

class Six
{
  public:
    Six ();
    static int parseEvaluate ( char[] command );
    static void runExecutor ();
    static int addActuator ( char[] description, actuatorType type, uint8_t numberElements=16, 
                      uint8_t baseAddress = 0x40, uint16_t frequency=1000, bool active=true, 
                      executionMode mode=EXECUTE_OFF, bool changed=false, 
                      int intensity=0, int parameter=0, int attribute=0);
    static uint8_t getNumberActuators ();
    static void begin ();
  private:
    static actuatorNode* _actuatorList = NULL;
    static actuator_t* _actuatorByUID ( uint8_t uid );
    static uint8_t _numberActuators;
}
