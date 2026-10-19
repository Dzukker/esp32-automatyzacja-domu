#pragma once
#include <Arduino.h>

struct EnvironmentData {
  float temperatureC = NAN;
  float humidityPct = NAN;
  float pressurePa = NAN;
  bool valid = false;
  uint32_t updatedAtMs = 0;
};

struct ClimateState {
  float targetTemperatureC = 22.0f;
  float currentTemperatureC = NAN;
  float temperatureDifferenceC = 0.0f;
  float coolingPower = 0.0f; // 0.0..1.0
};

enum class InputSource : uint8_t { LOCAL, REMOTE };
