#include <SPI.h>
#include <six.h>
#include <actuator.h>

six::Six sixthsense;

void setup() {
  Serial.begin(57600);

  sixthsense.begin();

  sixthsense.addActuator(
    // description
    "vibration armband - left arm",
    // type
    six::actuatorTypeClass::VIBRATION,
    // number of elements
    8,
    // # base address
    0x41
  );

  sixthsense.addActuator(
    // description
    "vibration armband - right arm",
    // type
    six::actuatorTypeClass::VIBRATION,
    // number of elements
    8,
    // # base address
    0x42
  );

  sixthsense.addActuator(
    // description
    "peltier sleeve - left arm",
    // type
    six::actuatorTypeClass::TEMPERATURE,
    // number of elements
    4,
    // # base address
    0x43
  );

  sixthsense.addActuator(
    // description
    "peltier sleeve - right arm",
    // type
    six::actuatorTypeClass::TEMPERATURE,
    // number of elements
    4,
    // # base address
    0x44
  );

  sixthsense.addActuator(
    // description
    "pressure armband - left arm",
    // type
    six::actuatorTypeClass::PRESSURE,
    // number of elements
    4,
    // # base address
    0x45
  );

  sixthsense.addActuator(
    // description
    "pressure armband - right arm",
    // type
    six::actuatorTypeClass::PRESSURE,
    // number of elements
    4,
    // # base address
    0x46
  );

  sixthsense.addActuator(
    // description
    "electro stimuation pads",
    // type
    six::actuatorTypeClass::ELECTRIC,
    // number of elements
    5,
    // # base address
    0x47
  );

}

ISR ( TIMER3_COMPA_vect ) {
  noInterrupts();
  sixthsense.timerIsr();
  interrupts();
}

void loop() {
  while(BLEMini.available()) {
    char c = (char)BLEMini.read();

    if ( c == '\r' || c == '\n' ) {
      sixthsense.parseEvaluate();
    }
    else { 
      sixthsense.addCommandCharacter(c);
    }
  }

  sixthsense.runExecutor();
}


// for debuging.................
char cmd[256];
uint8_t cmd_pos = 0;

void serialEvent() {
  while ( Serial.available() ) {
    char c = (char) Serial.read();
    if ( c == '\r' || c == '\n' ) {
      cmd[cmd_pos] = '\0';
      cmd_pos = 0;
      Serial.println();
      sixthsense.parseEvaluate(cmd);
      break;
    }
    else {
      cmd[cmd_pos] = c;
      Serial.print(c);
      cmd_pos++;
    }
  }
}
