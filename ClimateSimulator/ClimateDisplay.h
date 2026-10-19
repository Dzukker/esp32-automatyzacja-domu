#pragma once
#include <Arduino.h>
#include "ClimateTypes.h"

class ClimateDisplay {
public:
  void begin();
  void setSensorStatus(bool ok) { _sensorInitOk = ok; }
  void render(const EnvironmentData& env, const ClimateState& state,
              InputSource source, uint16_t potRaw, bool remoteFresh);
private:
  bool _sensorInitOk = false;
  uint32_t _lastDrawMs = 0;
};
