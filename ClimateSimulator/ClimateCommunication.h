#pragma once
#include <Arduino.h>
#include "ClimateTypes.h"

class ClimateCommunication {
public:
  void begin();
  void update();
  float remoteTargetC() const { return _remoteTargetC; }
  bool isRemoteFresh(uint32_t nowMs) const;
  bool consumeModeRequest(InputSource& source);
  void sendTelemetry(const EnvironmentData& env, const ClimateState& state, InputSource source);
  void sendEvent(const char* event);
private:
  static constexpr size_t RX_BUFFER_SIZE = 96;
  char _rxBuffer[RX_BUFFER_SIZE] = {};
  size_t _rxLength = 0;
  float _remoteTargetC = 22.0f;
  uint32_t _lastRemoteSetMs = 0;
  bool _hasRemoteTarget = false;
  bool _modeRequestPending = false;
  InputSource _requestedMode = InputSource::LOCAL;
  void processLine(char* line);
  void sendLine(const char* line);
};
