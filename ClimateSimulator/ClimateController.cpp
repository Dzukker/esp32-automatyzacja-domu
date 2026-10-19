#include "ClimateController.h"
#include "Config.h"

ClimateState ClimateController::update(const EnvironmentData& environment, float targetC) {
  ClimateState state;
  state.targetTemperatureC = constrain(targetC, MIN_TARGET_C, MAX_TARGET_C);
  state.currentTemperatureC = environment.valid ? environment.temperatureC : NAN;

  // Fail safe: no valid temperature => no simulated cooling output.
  if (!environment.valid || !isfinite(state.targetTemperatureC)) {
    _filteredPower = 0.0f;
    state.coolingPower = 0.0f;
    state.temperatureDifferenceC = NAN;
    return state;
  }

  state.temperatureDifferenceC = environment.temperatureC - state.targetTemperatureC;

  // Dead band avoids fan/LED flicker around the target temperature.
  float desired = 0.0f;
  if (state.temperatureDifferenceC > 0.3f) {
    desired = constrain(state.temperatureDifferenceC / FULL_COOLING_DELTA_C,
                        0.0f, 1.0f);
  }

  // Smooth changes, but ramp down faster than ramp up.
  const float alpha = (desired > _filteredPower) ? 0.18f : 0.35f;
  _filteredPower += alpha * (desired - _filteredPower);
  if (_filteredPower < 0.015f) _filteredPower = 0.0f;

  state.coolingPower = constrain(_filteredPower, 0.0f, 1.0f);
  return state;
}
