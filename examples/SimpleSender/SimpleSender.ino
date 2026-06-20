#include <EasyIR.h>

EasyIR ir;
const uint8_t txPin = 3; // IR LED with transistor on pin 3

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("EasyIR Simple Sender");
  ir.beginTx(txPin);
}

void loop() {
  Serial.println("Sending test NEC code 0x20DF10EF...");
  ir.sendNEC(0x20DF10EFUL, txPin);
  delay(3000);
}
