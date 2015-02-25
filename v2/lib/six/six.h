#include "execute.h"
#include "actuator.h"

class Six
{
  public:
    Six ();
    int parseEvaluate ( char[] command );
    int addActuator ( char[] description, actuatorType type, uint8_t numberElements=16, 
                      uint8_t baseAddress = 0x40, uint16_t frequency=1000, bool active=true, 
                      executionMode mode=EXECUTE_OFF, bool changed=false, 
                      int intensity=0, int parameter=0, int attribute=0);
    void runExecutor();
  private:
    actuatorNode* _actuatorList = NULL;
    actuator_t* _actuatorByUID ( uint8_t uid );
    uint8_t _numberActuators;
}
