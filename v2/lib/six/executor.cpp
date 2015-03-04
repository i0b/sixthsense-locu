#include <Arduino.h>
#include <QueueList.h>

#include "adafruit.h"
#include "actuator.h"
#include "executor.h"
#include "type.h"

namespace six {
  Executor::Executor(Actuator* actuator, Adafruit* adafruit){
    _actuator = actuator;
    _adafruit = adafruit;

    _executionQueue = new QueueList<_execution_t>();

    // initiallize heartbeat
    _connected = false;
    _keepAliveTimeout = 2000; // 200 * 10 ms = 2 sec
    _keepAliveTimer = _keepAliveTimeout;
  }

// -------------------------------------------------------------------------------------

  int Executor::runExecutor(){
    while(!_executionQueue->isEmpty()){
      _execution_t executionElement = _executionQueue->pop();
     ((executionElement.object)->*(executionElement.function))(executionElement.id, executionElement.intensity, executionElement.parameter);

     Serial.println("_executionQueue->pop()");

      /*
      Serial.print("executionQueue.count(): ");
      Serial.println(executionQueue.count()+1);

      Serial.print("actuatorUuid: ");
      Serial.println(executionElement.id);

      Serial.print("actuatorMode: ");
      Serial.println(EXECUTION_MODE_STRING [ actuator::actuators [ executionElement.id ].mode ]);

      Serial.print("executionIntensity: ");
      Serial.println(executionElement.intensity);

      Serial.print("executionParameter: ");
      Serial.println(executionElement.parameter);
      Serial.println();
      */
    }

    return 0;
  }

// -------------------------------------------------------------------------------------

  int Executor::ping(){
    if (_keepAliveTimer > 0){
      _keepAliveTimer = _keepAliveTimeout;
      _connected = true;
      return 0;
    }
    else {
      return -1;
    }
  }

// -------------------------------------------------------------------------------------

  int Executor::list(char* body, size_t& bodyLength){

    //for (uint8_t id = 0; id < _actuator->getNumberActuators(); id++){
      //Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);

    uint8_t id = 0;
    Actuator::actuator_t* anActuator;

    bodyLength=0;

    while ( (anActuator = _actuator->getActuatorById(id)) != NULL ) {
      bodyLength += snprintf(body+bodyLength, REQUEST_RESPONSE_PACKET_LENGTH-bodyLength, 
        "id: %d\r\n"
        "bus address: %x\r\n"
        "description: %s\r\n"
        "used channels: %d\r\n"
        //"actuator type: %s\r\n"
        "\r\n",
        id,
        anActuator->baseAddress,
        anActuator->description,
        anActuator->numberUsedChannels
        //actuatorTypeClassString[ anActuator->type ]
        );

      id++;
    }

    if (bodyLength < 0){
      return -1;
    }
    else { 
      return 0;
    }
  }

// -------------------------------------------------------------------------------------

  int Executor::demonstrateDisconnect(){
    // TODO which id?? -- not hard coded!!
    _execution_t executionElement = {this, 0, 0, 0, &six::Executor::_executeDisconnect };
    _executionQueue->push(executionElement);

    return 0;
  }

// -------------------------------------------------------------------------------------

  executionModeClass Executor::getMode(uint8_t id){
    if (id >= 0 && id <= _actuator->getNumberActuators()){
      Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);
      return anActuator->mode;
    }
    else {
      return executionModeClass::NONE;
    }
  }

// -------------------------------------------------------------------------------------

  int Executor::getIntensity(uint8_t id){
    if (id >= 0 && id <= _actuator->getNumberActuators()){
      Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);
      return anActuator->intensity;
    }
    else {
      return 0;
    }
  }

// -------------------------------------------------------------------------------------

  int Executor::getParameter(uint8_t id){
    if (id >= 0 && id <= _actuator->getNumberActuators()){
      Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);
      return anActuator->parameter;
    }
    else {
      return 0;
    }
  }

// -------------------------------------------------------------------------------------

  int Executor::setMode(uint8_t id, executionModeClass mode){
    Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);

    switch (mode){

      case executionModeClass::OFF :
        anActuator->mode = executionModeClass::OFF;
        break;

      case executionModeClass::TEMPERATURE :
        anActuator->mode = executionModeClass::TEMPERATURE;
        break;
      
      case executionModeClass::PRESSURE:
        anActuator->mode = executionModeClass::PRESSURE;
        break;

      case executionModeClass::VIBRATION :
        anActuator->mode = executionModeClass::VIBRATION;
        break;

      case executionModeClass::HEARTBEAT :
        anActuator->mode = executionModeClass::HEARTBEAT;
        break;

      case executionModeClass::ROTATION :
        anActuator->mode = executionModeClass::ROTATION;
        break;

      case executionModeClass::ELECTRO :
        anActuator->mode = executionModeClass::ELECTRO;
        break;
      
    }

    anActuator->changed = true;

    return 0;
  }

// -------------------------------------------------------------------------------------

  int Executor::setIntensity(uint8_t id, int intensity){
    Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);

    anActuator->intensity = intensity;
    anActuator->changed = true;

    return 0;
  }

// -------------------------------------------------------------------------------------

  int Executor::setParameter(uint8_t id, int parameter){
    Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);

    anActuator->parameter = parameter;
    anActuator->changed = true;

    return 0;
  }

// -------------------------------------------------------------------------------------

  void Executor::_executeOff(uint8_t id, int intensity, int parameter){
    Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);

    switch (anActuator->type){
      case (actuatorTypeClass::VIBRATION):
        setIntensity(id, 0);
        setParameter(id, 0);
        _executeVibration(id, 0, 0);
        break;
      case (actuatorTypeClass::TEMPERATURE):
        setIntensity(id, 0);
        setParameter(id, 0);
        _executeTemperature(id, 0, 0);
        break;
      case (actuatorTypeClass::PRESSURE):
        setIntensity(id, 90);
        setParameter(id, 0);
        _executeServo(id, 90, 0);
        break;
    }

    anActuator->changed = false;
  }

  void Executor::_executeServo(uint8_t id, int intensity, int parameter){
    Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);

    #define SERVOMIN  150 // this is the 'minimum' pulse length count(out of 4096)
    #define SERVOMAX  600 // this is the 'maximum' pulse length count(out of 4096)

    switch (anActuator->type){
      case (actuatorTypeClass::PRESSURE):
        uint16_t pulselen = map(anActuator->intensity, 0, 180, SERVOMIN, SERVOMAX);

        for (int channel = 0; channel < anActuator->numberUsedChannels; channel++){
          _adafruit->setPwm(anActuator->baseAddress, channel, 0, pulselen);
        }

        break;
    }

    anActuator->changed = false;
  }

  void Executor::_executeVibration(uint8_t id, int intensity, int parameter){
    Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);

    switch (anActuator->type){
      case (actuatorTypeClass::VIBRATION):
        for (int channel = 0; channel < anActuator->numberUsedChannels; channel++){
          if ((parameter >> channel)& 1 ){
            _adafruit->setPercent(anActuator->baseAddress, channel, intensity);
          }
          else {
            _adafruit->setPercent(anActuator->baseAddress, channel, _OFF);
          }
        }
        break;
    }

    anActuator->changed = false;
  }

  void Executor::_executeDisconnect(uint8_t id, int intensity, int parameter){
    Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);

    switch (anActuator->type){
      case (actuatorTypeClass::VIBRATION):
        for (int channel = 0; channel < anActuator->numberUsedChannels+1; channel++){
          if (channel > 0){
            _adafruit->setPercent(anActuator->baseAddress, channel-1, _OFF);
          }
          if (channel != anActuator->numberUsedChannels){
            _adafruit->setPercent(anActuator->baseAddress, channel,  _ON);
          }
          // CAREFUL!! USE OF DELAY
          delay(200);
        }
        break;
    }
  
    anActuator->changed = false;
  }

  void Executor::_executeTemperature(uint8_t id, int intensity, int parameter){
    Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);

    switch (anActuator->type){
      case (actuatorTypeClass::TEMPERATURE):
        Serial.println("DEBUG: executeTemperature()");
        for (uint8_t peltier = 0; peltier <(anActuator->numberUsedChannels / 3); peltier++){
          // parameter [ 0 ]: DIRECTION bitmap
          // parameter [ 1 ]: ON / OFF  bitmap
          // example: only peltier element 1 should be hot - parameter = { 0x0, 0x1 }
          if ((anActuator->intensity >> peltier)& 1 ){
            // hot 
            if ((anActuator->parameter >> peltier)& 1 ){
                _adafruit->setPercent(anActuator->baseAddress, 3*peltier+0,  _ON);
                _adafruit->setPercent(anActuator->baseAddress, 3*peltier+1, _OFF);
                _adafruit->setPercent(anActuator->baseAddress, 3*peltier+2,  _ON);
            }
            // cold
            else {
                _adafruit->setPercent(anActuator->baseAddress, 3 * peltier + 0, _OFF);
                _adafruit->setPercent(anActuator->baseAddress, 3 * peltier + 1,  _ON);
                _adafruit->setPercent(anActuator->baseAddress, 3 * peltier + 2,  _ON);
            }
          }
          else {
                // elementOffset: PIN1, PIN2, ENABLE
                _adafruit->setPercent(anActuator->baseAddress, 3 * peltier + 0, _OFF);
                _adafruit->setPercent(anActuator->baseAddress, 3 * peltier + 1, _OFF);
                _adafruit->setPercent(anActuator->baseAddress, 3 * peltier + 2, _OFF);
          }
        }
        break;
    }

    anActuator->changed = false;
  }

  void Executor::_electroStimulation(uint8_t id, int intensity, int parameter){
    Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);
    // parameter [ 0 ]: value of stimulation
    // parameter [ 1 ]: mode
    // FOR NOW: parameter [ 0 ]: 1 = LEFT, 2 = RIGHT, 3 = UP, 4 = DOWN, 5 = MODE
    #define  LEFT 2
    #define RIGHT 1
    #define    UP 0
    #define  DOWN 5
    #define  MODE 4

    switch (anActuator->type){

      case (actuatorTypeClass::ELECTRIC):
        switch (intensity){
          case (1):
            _adafruit->setPercent(anActuator->baseAddress,  LEFT, _OFF);
             delay(80);
            _adafruit->setPercent(anActuator->baseAddress,  LEFT,  _ON);
            break;
          case (2):
            _adafruit->setPercent(anActuator->baseAddress, RIGHT, _OFF);
             delay(80);
            _adafruit->setPercent(anActuator->baseAddress, RIGHT,  _ON);
            break;
          case (3):
            _adafruit->setPercent(anActuator->baseAddress,    UP, _OFF);
             delay(80);
            _adafruit->setPercent(anActuator->baseAddress,    UP,  _ON);
            break;
          case (4):
            _adafruit->setPercent(anActuator->baseAddress,  DOWN, _OFF);
             delay(80);
            _adafruit->setPercent(anActuator->baseAddress,  DOWN,  _ON);
            break;
          case (5):
            _adafruit->setPercent(anActuator->baseAddress,  MODE, _OFF);
             delay(80);
            _adafruit->setPercent(anActuator->baseAddress,  MODE,  _ON);
            break;
        }
        break;
    }

    anActuator->changed = false;
  }

// -----------------------------------------------------------------------------------------------
// ------------------------     INTERRUPT FUNCTIONS   --------------------------------------------
// -----------------------------------------------------------------------------------------------

  void Executor::timerIsr(){
    _executionTimer++;

    if (_connected && --_keepAliveTimer == 0){
    // TODO do disconnect pattern ;
    // TODO don't hardcode vibration element(s)
      _connected = false;

      //execution_t executionElementLeftVibration  = { 0, 0, 0, executeDisconnect };
      //executionQueue.push(executionElementLeftVibration);
      //executionT executionElementRightVibration = { 1, { 0 }, executeDisconnect };
      //executionQueue.push(executionElementRightVibration);
    }

    // DECISSION LOGIC
    //

    for (uint8_t id = 0; id < _actuator->getNumberActuators(); id++){
      Actuator::actuator_t* anActuator = _actuator->getActuatorById(id);

      int intensity = anActuator->intensity;
      int parameter = anActuator->parameter;

      switch (anActuator->mode){

        case (executionModeClass::OFF):
          if (anActuator->changed == true){
            anActuator->changed = false;
            _execution_t executionElement = {this, id, 0, 0, &six::Executor::_executeOff};
            _executionQueue->push(executionElement);
          }
          break;

        case (executionModeClass::TEMPERATURE):
          if (anActuator->type == actuatorTypeClass::TEMPERATURE
              && anActuator->changed == true){
            anActuator->changed = false;
            _execution_t executionElement = {this, id, intensity, 0, &six::Executor::_executeTemperature};
            _executionQueue->push(executionElement);
          }
          break;

        case (executionModeClass::PRESSURE):
          if (anActuator->type == actuatorTypeClass::PRESSURE
              && anActuator->changed == true){
            anActuator->changed = false;
            _execution_t executionElement = {this, id, intensity, 0, &six::Executor::_executeServo};
            _executionQueue->push(executionElement);
          }
          break;

        case (executionModeClass::VIBRATION):
          if (anActuator->type == actuatorTypeClass::VIBRATION
              && anActuator->changed == true){
            anActuator->changed = false;
            _execution_t executionElement = {this, id, intensity, parameter, &six::Executor::_executeVibration};
            _executionQueue->push(executionElement);
          }
          break;

        #define HEARTBEAT_VIBRATOR_ON_TIME 10
        case (executionModeClass::HEARTBEAT):
          if (anActuator->type == actuatorTypeClass::VIBRATION){
            if (anActuator->changed == true){
              anActuator->changed = false;
              _execution_t executionElement = {this, id, 0, 0, &six::Executor::_executeVibration};
              _executionQueue->push(executionElement);
            }

            uint32_t localInterval = _executionTimer %(HEARTBEAT_VIBRATOR_ON_TIME + parameter);

            // first beat high
            if (localInterval == 0){
              _execution_t executionElement = {this, id, intensity, 0xFF, &six::Executor::_executeVibration};
              _executionQueue->push(executionElement);
            }

            // first beat low
            else if (localInterval == HEARTBEAT_VIBRATOR_ON_TIME){
              _execution_t executionElement = {this, id, 0, 0, &six::Executor::_executeVibration};
              _executionQueue->push(executionElement);
            }
          }
          break;

        case (executionModeClass::ROTATION):
          if (anActuator->type == actuatorTypeClass::VIBRATION){
            if (anActuator->changed == true){
              anActuator->changed = false;
              _execution_t executionElement = {this, id, 0, 0, &six::Executor::_executeVibration};
              _executionQueue->push(executionElement);
            }

            uint32_t localInterval = _executionTimer % parameter;

            if (localInterval == 0){
              uint8_t activeChannel =(anActuator->attribute + 1)% anActuator->numberUsedChannels;
              anActuator->attribute = activeChannel;

              uint8_t channelBitmap = 1;

              for (uint8_t channel = 0; channel < activeChannel; channel++){
                channelBitmap <<= 1;
              }

              _execution_t executionElement = {this, id, intensity, channelBitmap, &six::Executor::_executeVibration};
              _executionQueue->push(executionElement);
            }
          }
          break;

        case (executionModeClass::ELECTRO):
          if (anActuator->type == actuatorTypeClass::ELECTRIC
              && anActuator->changed == true){
            anActuator->changed = false;
            _execution_t executionElement = {this, id, intensity, 0, &six::Executor::_electroStimulation};
            _executionQueue->push(executionElement);
          }
          break;

      }

    }

  }

// -----------------------------------------------------------------------------------------------

}
