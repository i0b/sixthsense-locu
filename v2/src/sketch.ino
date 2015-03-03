#include <SPI.h>
#include <six.h>
#include <actuator.h>

six::Six sixthsense;

void setup() {
  Serial.begin(57600);

  sixthsense.addActuator(
    // description
    "vibration armband - left arm",
    // type
    six::actuatorType::VIBRATION,
    // number of elements
    8,
    // # base address
    0x41
  );

  sixthsense.addActuator(
    // description
    "vibration armband - right arm",
    // type
    six::actuatorType::VIBRATION,
    // number of elements
    8,
    // # base address
    0x42
  );

  sixthsense.addActuator(
    // description
    "peltier armband - left arm",
    // type
    six::actuatorType::TEMPERATURE,
    // number of elements
    12,
    // # base address
    0x43
  );

  sixthsense.addActuator(
    // description
    "peltier armband - right arm",
    // type
    six::actuatorType::TEMPERATURE,
    // number of elements
    12,
    // # base address
    0x44
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
