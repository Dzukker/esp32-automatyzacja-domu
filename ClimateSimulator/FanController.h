#pragma once
#include <Arduino.h>

class FanController {
public:
  void begin();
  void setPower(float power01);
  void update(uint32_t nowMs);
private:
  float _requestedPower = 0.0f;
  uint8_t _duty = 0;
  bool _started = false;
  uint32_t _lastChangeMs = 0;
  void writeDuty(uint8_t duty);
};
