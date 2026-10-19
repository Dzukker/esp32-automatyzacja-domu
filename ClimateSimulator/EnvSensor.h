#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <M5UnitENV.h>
#include "ClimateTypes.h"

class EnvSensor {
public:
  bool begin();
  EnvironmentData read();
  bool isReady() const { return _ready; }
private:
  SHT3X _sht30;
  QMP6988 _qmp6988;
  bool _ready = false;
};
