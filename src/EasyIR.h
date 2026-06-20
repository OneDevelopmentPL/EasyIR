#ifndef EASYIR_H
#define EASYIR_H

#include <Arduino.h>

class EasyIR {
public:
  EasyIR();
  void begin(uint8_t rxPin);
  int readRaw(unsigned long *buffer, int maxLen, unsigned long timeout = 100000UL);
  bool decodeNEC(const unsigned long *buffer, int len, uint32_t &result);
  void beginTx(uint8_t txPin);
  void sendNEC(uint32_t data, uint8_t txPin);

private:
  uint8_t _rxPin = 255;
  uint8_t _txPin = 255;
  void mark(unsigned int time, uint8_t pin);
  void space(unsigned int time, uint8_t pin);
  bool approx(unsigned long measured, unsigned long expected, unsigned long tolerancePercent = 35);
};

#endif
