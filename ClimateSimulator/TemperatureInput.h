#pragma once
#include <Arduino.h>

class TemperatureInput {
public:
  void begin();
  float readTargetC();
  uint16_t rawValue() const { return _raw; }
private:
  uint16_t _raw = 0;
  float _filtered = 0;
  bool _initialized = false;
};
