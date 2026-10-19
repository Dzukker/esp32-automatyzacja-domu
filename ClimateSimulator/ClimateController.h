#pragma once
#include <Arduino.h>
#include "ClimateTypes.h"

class ClimateController {
public:
  ClimateState update(const EnvironmentData& environment, float targetC);
private:
  float _filteredPower = 0.0f;
};
