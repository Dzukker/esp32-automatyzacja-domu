#pragma once
#include <Arduino.h>
#include "ClimateTypes.h"

class ClimateDisplay {
public:
  void begin();
  void setSensorStatus(bool ok) { _sensorInitOk = ok; }
  void render(const EnvironmentData& env, const ClimateState& state,
              float targetC, UiMode mode, uint32_t selectedDurationSeconds,
              uint32_t remainingSeconds, bool stopChoiceYes);
private:
  bool _sensorInitOk = false;
  bool _hasDrawn = false;
  UiMode _lastMode = UiMode::MONITOR;
  void drawHeader(const char* title);
  void printDuration(uint32_t seconds, char* out, size_t outSize);
};
