#include <M5Unified.h>
#include <M5UnitENV.h>
#include <Adafruit_NeoPixel.h>
#include "Config.h"
#include "ClimateTypes.h"
#include "EnvSensor.h"
#include "TemperatureInput.h"
#include "ClimateController.h"
#include "FanController.h"
#include "LedRingController.h"
#include "ClimateDisplay.h"
#include "ClimateCommunication.h"

EnvSensor envSensor;
TemperatureInput temperatureInput;
ClimateController climateController;
FanController fanController;
LedRingController ledRing;
ClimateDisplay displayUi;
ClimateCommunication communication;

EnvironmentData environmentData;
ClimateState climateState;
InputSource inputSource = InputSource::LOCAL;

uint32_t lastSensorMs = 0;
uint32_t lastControlMs = 0;
uint32_t lastDisplayMs = 0;
uint32_t lastTelemetryMs = 0;

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  Serial.begin(115200);
  delay(100);

  M5.Display.setRotation(1);
  M5.Display.setBrightness(150);

  temperatureInput.begin();
  ledRing.begin();
  fanController.begin();
  communication.begin();

  const bool sensorOk = envSensor.begin();
  environmentData.valid = false;

  climateState.targetTemperature = DEFAULT_TARGET_C;
  climateState.currentTemperature = NAN;
  climateState.temperatureDifference = 0;
  climateState.coolingPower = 0;

  displayUi.begin();
  displayUi.setSensorStatus(sensorOk);

  Serial.println("\nClimateSimulator boot");
  Serial.println("UART commands: SET,<degC>; MODE,LOCAL; MODE,REMOTE; PING");
  Serial.println("Press M5 button A to switch LOCAL/REMOTE source.");
}

void loop() {
  M5.update();
  const uint32_t now = millis();

  communication.update();

  // Button A toggles the active target source.
  if (M5.BtnA.wasPressed()) {
    inputSource = (inputSource == InputSource::LOCAL)
                    ? InputSource::REMOTE : InputSource::LOCAL;
    communication.sendEvent(inputSource == InputSource::LOCAL
                              ? "MODE,LOCAL" : "MODE,REMOTE");
  }

  if (communication.consumeModeRequest(inputSource)) {
    // Remote may explicitly request the active source with MODE,LOCAL/REMOTE.
  }

  if (now - lastSensorMs >= SENSOR_INTERVAL_MS) {
    lastSensorMs = now;
    environmentData = envSensor.read();
  }

  const float localTarget = temperatureInput.readTargetC();
  const float target = (inputSource == InputSource::LOCAL)
                         ? localTarget : communication.remoteTargetC();

  if (now - lastControlMs >= CONTROL_INTERVAL_MS) {
    lastControlMs = now;
    // In REMOTE mode, stale/missing commands fail safe to zero cooling.
    if (inputSource == InputSource::REMOTE && !communication.isRemoteFresh(now)) {
      climateState = climateController.update(environmentData, MAX_TARGET_C);
      climateState.targetTemperatureC = communication.remoteTargetC();
      climateState.coolingPower = 0.0f;
    } else {
      climateState = climateController.update(environmentData, target);
    }
    fanController.setPower(climateState.coolingPower);
    ledRing.setPower(climateState.coolingPower);
  }

  fanController.update(now);
  ledRing.update(now);

  if (now - lastDisplayMs >= DISPLAY_INTERVAL_MS) {
    lastDisplayMs = now;
    displayUi.render(environmentData, climateState, inputSource,
                     temperatureInput.rawValue(), communication.isRemoteFresh(now));
  }

  if (now - lastTelemetryMs >= TELEMETRY_INTERVAL_MS) {
    lastTelemetryMs = now;
    communication.sendTelemetry(environmentData, climateState, inputSource);
  }
}
