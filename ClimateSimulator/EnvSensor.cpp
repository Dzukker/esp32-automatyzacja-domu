#include "EnvSensor.h"
#include "Config.h"

bool EnvSensor::begin() {
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 400000U);
  const bool qmpOk = _qmp6988.begin(&Wire, QMP6988_SLAVE_ADDRESS_L,
                                    PIN_I2C_SDA, PIN_I2C_SCL, 400000U);
  const bool shtOk = _sht30.begin(&Wire, SHT3X_I2C_ADDR,
                                  PIN_I2C_SDA, PIN_I2C_SCL, 400000U);
  _ready = qmpOk && shtOk;
  if (!_ready) {
    Serial.printf("ENV III init failed: SHT30=%d QMP6988=%d\n", shtOk, qmpOk);
  }
  return _ready;
}

EnvironmentData EnvSensor::read() {
  EnvironmentData out;
  const bool shtOk = _sht30.update();
  const bool qmpOk = _qmp6988.update();

  if (shtOk && qmpOk) {
    out.temperatureC = _sht30.cTemp;
    out.humidityPct = _sht30.humidity;
    out.pressurePa = _qmp6988.pressure;
    out.valid = isfinite(out.temperatureC) && isfinite(out.humidityPct)
                && isfinite(out.pressurePa)
                && out.temperatureC > -40.0f && out.temperatureC < 120.0f
                && out.humidityPct >= 0.0f && out.humidityPct <= 100.0f
                && out.pressurePa > 30000.0f && out.pressurePa < 120000.0f;
    out.updatedAtMs = millis();
  }
  return out;
}
