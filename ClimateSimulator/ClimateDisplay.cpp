#include "ClimateDisplay.h"
#include <M5Unified.h>

void ClimateDisplay::begin() {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
}

void ClimateDisplay::render(const EnvironmentData& env, const ClimateState& state,
                            InputSource source, uint16_t potRaw, bool remoteFresh) {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(8, 6);
  M5.Display.setTextColor(TFT_CYAN, TFT_BLACK);
  M5.Display.println("CLIMATE SIMULATOR");
  M5.Display.drawFastHLine(8, 31, M5.Display.width() - 16, TFT_DARKGREY);

  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setCursor(8, 42);
  M5.Display.printf("Temp: ");
  if (env.valid) M5.Display.printf("%.1f C\n", env.temperatureC);
  else M5.Display.println("--.- C");

  M5.Display.setCursor(8, 70);
  M5.Display.printf("Humidity: ");
  if (env.valid) M5.Display.printf("%.1f %%\n", env.humidityPct);
  else M5.Display.println("--.- %");

  M5.Display.setCursor(8, 98);
  M5.Display.printf("Pressure: ");
  if (env.valid) M5.Display.printf("%.1f hPa\n", env.pressurePa / 100.0f);
  else M5.Display.println("----.- hPa");

  M5.Display.drawFastHLine(8, 128, M5.Display.width() - 16, TFT_DARKGREY);
  M5.Display.setCursor(8, 138);
  M5.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
  M5.Display.printf("Target: %.1f C\n", state.targetTemperatureC);

  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setCursor(8, 166);
  M5.Display.printf("Source: %s\n", source == InputSource::LOCAL ? "LOCAL POT" : "REMOTE UART");

  M5.Display.setCursor(8, 194);
  M5.Display.printf("Cooling: %3.0f %%\n", state.coolingPower * 100.0f);

  M5.Display.setCursor(8, 222);
  M5.Display.printf("Delta: ");
  if (isfinite(state.temperatureDifferenceC))
    M5.Display.printf("%+.1f C\n", state.temperatureDifferenceC);
  else M5.Display.println("--.- C");

  M5.Display.setTextSize(1);
  M5.Display.setCursor(8, M5.Display.height() - 18);
  M5.Display.setTextColor(_sensorInitOk && env.valid ? TFT_GREEN : TFT_RED, TFT_BLACK);
  M5.Display.printf("ENV: %s | UART remote: %s | POT ADC: %u",
    (_sensorInitOk && env.valid) ? "OK" : "CHECK",
    remoteFresh ? "fresh" : "stale",
    potRaw);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(2);
}
