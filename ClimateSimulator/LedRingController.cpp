#include "LedRingController.h"
#include "Config.h"
#include <math.h>

LedRingController::LedRingController()
  : _pixels(LED_COUNT, PIN_LED_DATA, NEO_GRB + NEO_KHZ800) {}

void LedRingController::begin() {
  _pixels.begin();
  _pixels.setBrightness(LED_BRIGHTNESS);
  _pixels.clear();
  _pixels.show();
}

void LedRingController::setPower(float power01) {
  _power = constrain(power01, 0.0f, 1.0f);
}

uint32_t LedRingController::colorForPower(float p) {
  p = constrain(p, 0.0f, 1.0f);
  // 0 = blue, 0.5 = yellow, 1 = red.
  uint8_t r, g, b;
  if (p < 0.5f) {
    float t = p * 2.0f;
    r = static_cast<uint8_t>(255.0f * t);
    g = static_cast<uint8_t>(255.0f * t);
    b = static_cast<uint8_t>(255.0f * (1.0f - t));
  } else {
    float t = (p - 0.5f) * 2.0f;
    r = 255;
    g = static_cast<uint8_t>(255.0f * (1.0f - t));
    b = 0;
  }
  return _pixels.Color(r, g, b);
}

void LedRingController::update(uint32_t nowMs) {
  if (nowMs - _lastFrameMs < 40) return;
  _lastFrameMs = nowMs;

  _activeCount = (_power <= 0.01f) ? 0 :
    static_cast<uint8_t>(ceil(_power * LED_COUNT));
  const uint32_t color = colorForPower(_power);

  for (uint8_t i = 0; i < LED_COUNT; ++i) {
    if (i < _activeCount) _pixels.setPixelColor(i, color);
    else _pixels.setPixelColor(i, 0);
  }
  _pixels.show();
}
