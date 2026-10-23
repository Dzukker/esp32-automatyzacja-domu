#include <M5Unified.h>
#include <M5UnitENV.h>
#include "Config.h"
#include "ClimateTypes.h"
#include "EnvSensor.h"
#include "ClimateController.h"
#include "LedRingController.h"
#include "ClimateDisplay.h"

EnvSensor envSensor;
ClimateController climateController;
LedRingController ledRing;
ClimateDisplay displayUi;

EnvironmentData environmentData;
ClimateState climateState;

float targetTemperatureC = DEFAULT_TARGET_C;
uint32_t selectedDurationSeconds = MIN_DURATION_SECONDS;
uint32_t coolingEndsAtMs = 0;
UiMode uiMode = UiMode::MONITOR;
bool customConfigurationActive = false;
bool stopChoiceYes = true;

uint32_t lastSensorMs = 0;
uint32_t lastControlMs = 0;
uint32_t lastDisplayMs = 0;

void resetCustomConfiguration() {
  targetTemperatureC = DEFAULT_TARGET_C;
  selectedDurationSeconds = MIN_DURATION_SECONDS;
  coolingEndsAtMs = 0;
  customConfigurationActive = false;
  climateState.coolingPower = 0.0f;
  climateController.update(environmentData, targetTemperatureC);
  ledRing.setPower(0.0f);
  uiMode = UiMode::MONITOR;
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  Serial.begin(115200);
  delay(100);

  M5.Display.setRotation(1);
  M5.Display.setBrightness(150);

  ledRing.begin();
  const bool sensorOk = envSensor.begin();
  environmentData.valid = false;

  climateState.targetTemperatureC = targetTemperatureC;
  climateState.currentTemperatureC = NAN;
  climateState.temperatureDifferenceC = NAN;
  climateState.coolingPower = 0.0f;

  displayUi.begin();
  displayUi.setSensorStatus(sensorOk);

  Serial.println("\nClimateSimulator");
  Serial.println("Monitor until any button is pressed.");
  Serial.println("Temp menu: A warmer, B cooler, C confirm.");
  Serial.println("Duration menu: A +10 min, B -10 min, C start.");
}

void loop() {
  M5.update();
  const uint32_t now = millis();

  const bool pressedA = M5.BtnA.wasPressed();
  const bool pressedB = M5.BtnB.wasPressed();
  const bool pressedC = M5.BtnC.wasPressed();
  const bool anyPressed = pressedA || pressedB || pressedC;

  switch (uiMode) {
    case UiMode::MONITOR:
      // The first press opens configuration; it does not change a value.
      if (anyPressed) {
        uiMode = UiMode::SET_TEMPERATURE;
      }
      break;

    case UiMode::SET_TEMPERATURE:
      if (pressedA) {
        targetTemperatureC = constrain(targetTemperatureC + TARGET_STEP_C,
                                       MIN_TARGET_C, MAX_TARGET_C);
      }
      if (pressedB) {
        targetTemperatureC = constrain(targetTemperatureC - TARGET_STEP_C,
                                       MIN_TARGET_C, MAX_TARGET_C);
      }
      if (pressedC) {
        uiMode = UiMode::SET_DURATION;
      }
      break;

    case UiMode::SET_DURATION:
      if (pressedA) {
        selectedDurationSeconds = min(selectedDurationSeconds + DURATION_STEP_SECONDS,
                                       MAX_DURATION_SECONDS);
      }
      if (pressedB && selectedDurationSeconds > MIN_DURATION_SECONDS) {
        selectedDurationSeconds -= DURATION_STEP_SECONDS;
      }
      if (pressedC) {
        coolingEndsAtMs = now + selectedDurationSeconds * 1000UL;
        customConfigurationActive = true;
        uiMode = UiMode::COOLING;
      }
      break;

    case UiMode::COOLING:
      if (pressedC) {
        stopChoiceYes = true;
        uiMode = UiMode::STOP_CONFIRM;
      } else if ((int32_t)(now - coolingEndsAtMs) >= 0) {
        customConfigurationActive = false;
        uiMode = UiMode::COMPLETE;
      }
      break;

    case UiMode::STOP_CONFIRM:
      if (pressedA) {
        // YES — anuluj sesję i usuń konfigurację
        resetCustomConfiguration();
      } else if (pressedB) {
        // NO — zachowaj konfigurację i wróć do chłodzenia
        uiMode = UiMode::COOLING;
      }
      break;

    case UiMode::COMPLETE:
      // C returns to monitoring; any button then opens the setup menu.
      if (anyPressed) {
        resetCustomConfiguration();
      }
      break;
  }

  if (now - lastSensorMs >= SENSOR_INTERVAL_MS) {
    lastSensorMs = now;
    environmentData = envSensor.read();
  }

  if (now - lastControlMs >= CONTROL_INTERVAL_MS) {
    lastControlMs = now;
    if (uiMode == UiMode::COOLING) {
      climateState = climateController.update(environmentData, targetTemperatureC);
      ledRing.setPower(climateState.coolingPower);
    } else if (uiMode == UiMode::MONITOR) {
      // Idle screen displays the estimated cooling demand against the default target.
      climateState = climateController.update(environmentData, DEFAULT_TARGET_C);
      climateState.targetTemperatureC = DEFAULT_TARGET_C;
      ledRing.setPower(climateState.coolingPower);
    } else {
      climateState = climateController.update(environmentData, targetTemperatureC);
      climateState.targetTemperatureC = targetTemperatureC;
      if (uiMode == UiMode::STOP_CONFIRM) {
        // Keep the active cooling simulation running while the prompt is displayed.
        ledRing.setPower(climateState.coolingPower);
      } else {
        climateState.coolingPower = 0.0f;
        ledRing.setPower(0.0f);
      }
    }
  }

  ledRing.update(now);

  if (now - lastDisplayMs >= DISPLAY_INTERVAL_MS) {
    lastDisplayMs = now;
    uint32_t remainingSeconds = 0;
    if (uiMode == UiMode::COOLING || uiMode == UiMode::STOP_CONFIRM) {
      if ((int32_t)(coolingEndsAtMs - now) > 0) {
        remainingSeconds = (coolingEndsAtMs - now + 999UL) / 1000UL;
      }
    }
    displayUi.render(environmentData, climateState, targetTemperatureC,
                     uiMode, selectedDurationSeconds, remainingSeconds,
                     stopChoiceYes);
  }
}
