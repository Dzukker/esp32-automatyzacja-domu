#include "FanController.h"
#include "Config.h"

void FanController::begin() {
  pinMode(PIN_FAN_PWM, OUTPUT);
  digitalWrite(PIN_FAN_PWM, LOW);

  // Arduino-ESP32 core 3.x
  _started = ledcAttach(
    PIN_FAN_PWM,
    FAN_PWM_HZ,
    FAN_PWM_BITS
  );

  if (!_started) {
    Serial.println("ERROR: LEDC attach failed");
    digitalWrite(PIN_FAN_PWM, LOW);
    return;
  }

  writeDuty(0);
}

void FanController::setPower(float power01) {
  _requestedPower = constrain(power01, 0.0f, 1.0f);
}

void FanController::update(uint32_t nowMs) {
  if (!_started) {
    digitalWrite(PIN_FAN_PWM, LOW);
    return;
  }

  if (nowMs - _lastChangeMs < 50) return;
  _lastChangeMs = nowMs;

  uint8_t targetDuty = 0;

  if (_requestedPower > 0.01f) {
    const float minDuty =
      FAN_MIN_START_PERCENT / 100.0f;

    const float effective =
      minDuty + _requestedPower * (1.0f - minDuty);

    targetDuty = static_cast<uint8_t>(
      constrain(effective, 0.0f, 1.0f) * 255.0f
    );
  }

  // Limit changes in duty cycle.
  if (_duty < targetDuty) {
    _duty = (targetDuty - _duty > 12)
      ? _duty + 12 : targetDuty;
  } else if (_duty > targetDuty) {
    _duty = (_duty - targetDuty > 12)
      ? _duty - 12 : targetDuty;
  }

  writeDuty(_duty);
}

void FanController::writeDuty(uint8_t duty) {
  _duty = duty;

  if (_started) {
    // Core 3.x: pass the GPIO pin, not a channel.
    ledcWrite(PIN_FAN_PWM, duty);
  }
}