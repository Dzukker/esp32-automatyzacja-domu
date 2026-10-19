#include "TemperatureInput.h"
#include "Config.h"

void TemperatureInput::begin() {
  analogReadResolution(12);
  analogSetPinAttenuation(PIN_POT_ADC, ADC_11db);
  pinMode(PIN_POT_ADC, INPUT);
}

float TemperatureInput::readTargetC() {
  // Small oversampling + low-pass filter to reduce jitter.
  uint32_t sum = 0;
  for (int i = 0; i < 8; ++i) sum += analogRead(PIN_POT_ADC);
  _raw = static_cast<uint16_t>(sum / 8U);

  if (!_initialized) {
    _filtered = _raw;
    _initialized = true;
  } else {
    _filtered = 0.20f * _raw + 0.80f * _filtered;
  }

  const float normalized = constrain(_filtered, 0.0f, 4095.0f) / 4095.0f;
  return MIN_TARGET_C + normalized * (MAX_TARGET_C - MIN_TARGET_C);
}
