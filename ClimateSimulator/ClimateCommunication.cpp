#include "ClimateCommunication.h"
#include "Config.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void ClimateCommunication::begin() {
  Serial2.begin(UART_BAUD, SERIAL_8N1, PIN_UART_RX, PIN_UART_TX);
  _remoteTargetC = DEFAULT_TARGET_C;
}

void ClimateCommunication::update() {
  while (Serial2.available() > 0) {
    const char c = static_cast<char>(Serial2.read());
    if (c == '\r') continue;
    if (c == '\n') {
      _rxBuffer[_rxLength] = '\0';
      if (_rxLength > 0) processLine(_rxBuffer);
      _rxLength = 0;
    } else if (_rxLength < RX_BUFFER_SIZE - 1) {
      _rxBuffer[_rxLength++] = c;
    } else {
      _rxLength = 0; // discard overlong line
    }
  }
}

void ClimateCommunication::processLine(char* line) {
  if (strncmp(line, "SET,", 4) == 0) {
    char* end = nullptr;
    const float value = strtof(line + 4, &end);
    if (end != line + 4 && *end == '\0' && isfinite(value)
        && value >= MIN_TARGET_C && value <= MAX_TARGET_C) {
      _remoteTargetC = value;
      _lastRemoteSetMs = millis();
      _hasRemoteTarget = true;
      sendLine("ACK,SET");
    } else {
      sendLine("ERR,SET_RANGE");
    }
  } else if (strcmp(line, "MODE,LOCAL") == 0) {
    _requestedMode = InputSource::LOCAL;
    _modeRequestPending = true;
    sendLine("ACK,MODE");
  } else if (strcmp(line, "MODE,REMOTE") == 0) {
    _requestedMode = InputSource::REMOTE;
    _modeRequestPending = true;
    sendLine("ACK,MODE");
  } else if (strcmp(line, "PING") == 0) {
    sendLine("PONG");
  } else {
    sendLine("ERR,UNKNOWN_COMMAND");
  }
}

bool ClimateCommunication::isRemoteFresh(uint32_t nowMs) const {
  return _hasRemoteTarget && (nowMs - _lastRemoteSetMs <= REMOTE_TIMEOUT_MS);
}

bool ClimateCommunication::consumeModeRequest(InputSource& source) {
  if (!_modeRequestPending) return false;
  source = _requestedMode;
  _modeRequestPending = false;
  return true;
}

void ClimateCommunication::sendLine(const char* line) {
  Serial2.print(line);
  Serial2.print('\n');
}

void ClimateCommunication::sendEvent(const char* event) {
  sendLine(event);
}

void ClimateCommunication::sendTelemetry(const EnvironmentData& env,
                                         const ClimateState& state,
                                         InputSource source) {
  char line[160];
  if (env.valid) {
    snprintf(line, sizeof(line), "ENV,%.2f,%.2f,%.0f,%.2f,%.3f,%s",
      env.temperatureC, env.humidityPct, env.pressurePa,
      state.targetTemperatureC, state.coolingPower,
      source == InputSource::LOCAL ? "LOCAL" : "REMOTE");
  } else {
    snprintf(line, sizeof(line), "ENV,NaN,NaN,NaN,%.2f,%.3f,%s",
      state.targetTemperatureC, state.coolingPower,
      source == InputSource::LOCAL ? "LOCAL" : "REMOTE");
  }
  sendLine(line);
}
