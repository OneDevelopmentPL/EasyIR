#include <EasyIR.h>

EasyIR ir;
unsigned long buf[100];

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("EasyIR Simple Receiver");
  ir.begin(2); // RX pin 2 by default, change as needed
}

void loop() {
  int len = ir.readRaw(buf, 100, 200000UL);
  if (len > 0) {
    Serial.print("Captured pulses: "); Serial.println(len);
    for (int i = 0; i < len; ++i) {
      Serial.print(buf[i]);
      if (i + 1 < len) Serial.print(", ");
    }
    Serial.println();

    uint32_t code;
    if (ir.decodeNEC(buf, len, code)) {
      Serial.print("NEC code: 0x"); Serial.println(code, HEX);
    } else {
      Serial.println("Not NEC or decode failed");
    }
  }
  delay(200);
}
