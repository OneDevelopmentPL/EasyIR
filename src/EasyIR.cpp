#include "EasyIR.h"

EasyIR::EasyIR() {}

void EasyIR::begin(uint8_t rxPin) {
  _rxPin = rxPin;
  pinMode(_rxPin, INPUT);
}

int EasyIR::readRaw(unsigned long *buffer, int maxLen, unsigned long timeout) {
  if (_rxPin == 255) return 0;
  int idx = 0;
  unsigned long t;

  // wait for a pulse to start (mark)
  unsigned long start = micros();
  while (digitalRead(_rxPin) == LOW) {
    if (micros() - start > timeout) return 0;
  }

  // capture alternating HIGH/LOW durations
  while (idx < maxLen) {
    t = pulseIn(_rxPin, HIGH, timeout);
    if (t == 0) break;
    buffer[idx++] = t;
    if (idx >= maxLen) break;
    t = pulseIn(_rxPin, LOW, timeout);
    if (t == 0) break;
    buffer[idx++] = t;
  }
  return idx;
}

bool EasyIR::approx(unsigned long measured, unsigned long expected, unsigned long tolerancePercent) {
  long diff = (long)measured - (long)expected;
  if (diff < 0) diff = -diff;
  return (unsigned long)diff <= (expected * tolerancePercent) / 100UL;
}

bool EasyIR::decodeNEC(const unsigned long *buffer, int len, uint32_t &result) {
  // NEC minimum: leader (2 entries) + 32 bits * 2 (mark+space) = 66 entries
  if (len < 66) return false;
  int i = 0;
  // Leader mark ~9000, leader space ~4500
  if (!approx(buffer[i++], 9000)) return false;
  if (!approx(buffer[i++], 4500)) return false;

  uint32_t value = 0;
  for (int bit = 0; bit < 32; ++bit) {
    if (i + 1 >= len) return false;
    unsigned long markDur = buffer[i++];
    unsigned long spaceDur = buffer[i++];
    if (!approx(markDur, 560, 60)) return false; // mark approx 560
    // space: ~560 = 0, ~1690 = 1
    if (approx(spaceDur, 560, 60)) {
      // bit 0
      value <<= 1;
    } else if (approx(spaceDur, 1690, 50)) {
      value = (value << 1) | 1;
    } else {
      return false;
    }
  }
  result = value;
  return true;
}

void EasyIR::beginTx(uint8_t txPin) {
  _txPin = txPin;
  pinMode(_txPin, OUTPUT);
  digitalWrite(_txPin, LOW);
}

// generate carrier for 'time' microseconds (approx 38kHz)
void EasyIR::mark(unsigned int time, uint8_t pin) {
  unsigned long start = micros();
  const unsigned int halfPeriod = 13; // ~38kHz -> 26us period -> 13us half
  while (micros() - start < time) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(halfPeriod);
    digitalWrite(pin, LOW);
    delayMicroseconds(halfPeriod);
  }
}

void EasyIR::space(unsigned int time, uint8_t pin) {
  digitalWrite(pin, LOW);
  if (time > 0) delayMicroseconds(time);
}

void EasyIR::sendNEC(uint32_t data, uint8_t txPin) {
  if (txPin == 255) return;
  // leader
  mark(9000, txPin);
  space(4500, txPin);

  // 32 bits, MSB first as typical raw value
  for (int i = 31; i >= 0; --i) {
    mark(560, txPin);
    if (data & (1UL << i)) {
      space(1690, txPin);
    } else {
      space(560, txPin);
    }
  }

  // final mark
  mark(560, txPin);
  space(0, txPin);
}
