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
  float temperatureDifferenceC = NAN;
  float coolingPower = 0.0f;
};

enum class UiMode : uint8_t {
  MONITOR,
  SET_TEMPERATURE,
  SET_DURATION,
  COOLING,
  STOP_CONFIRM,
  COMPLETE
};
