#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class LedRingController {
public:
  LedRingController();
  void begin();
  void setPower(float power01);
  void update(uint32_t nowMs);
private:
  Adafruit_NeoPixel _pixels;
  float _power = 0.0f;
  uint32_t _lastFrameMs = 0;
  uint8_t _activeCount = 0;
  uint32_t colorForPower(float p);
};
