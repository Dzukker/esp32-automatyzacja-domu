#include "ClimateDisplay.h"
#include "Config.h"
#include <M5Unified.h>

void ClimateDisplay::begin() {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  _hasDrawn = false;
}

void ClimateDisplay::drawHeader(const char* title) {
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(TFT_CYAN, TFT_BLACK);
  M5.Display.setCursor(8, 8);
  M5.Display.printf("%-22s", title);
  M5.Display.drawFastHLine(8, 34, M5.Display.width() - 16, TFT_DARKGREY);
}

void ClimateDisplay::printDuration(uint32_t seconds, char* out, size_t outSize) {
  const uint32_t hours = seconds / 3600UL;
  const uint32_t minutes = (seconds % 3600UL) / 60UL;
  snprintf(out, outSize, "%lu:%02lu",
           (unsigned long)hours, (unsigned long)minutes);
}

void ClimateDisplay::render(const EnvironmentData& env, const ClimateState& state,
                            float targetC, UiMode mode,
                            uint32_t selectedDurationSeconds,
                            uint32_t remainingSeconds, bool stopChoiceYes) {
  // Clear only on screen/mode transitions to avoid visible full-screen flicker.
  if (!_hasDrawn || mode != _lastMode) {
    M5.Display.fillScreen(TFT_BLACK);
    if (mode == UiMode::MONITOR) drawHeader("CLIMATE MONITOR");
    else if (mode == UiMode::SET_TEMPERATURE) drawHeader("SET TEMPERATURE");
    else if (mode == UiMode::SET_DURATION) drawHeader("COOLING DURATION");
    else if (mode == UiMode::COOLING) drawHeader("COOLING ACTIVE");
    else if (mode == UiMode::STOP_CONFIRM) drawHeader("STOP CONFIG?");
    else drawHeader("COOLING COMPLETE");
    _lastMode = mode;
    _hasDrawn = true;
  }

  M5.Display.setTextSize(2);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);

  if (mode == UiMode::MONITOR) {
    M5.Display.setCursor(8, 48);
    M5.Display.printf("Temp: ");
    if (env.valid) M5.Display.printf("%.1f C       ", env.temperatureC);
    else M5.Display.print("--.- C       ");
    M5.Display.setCursor(8, 78);
    if (env.valid) M5.Display.printf("Humidity: %.1f %%   ", env.humidityPct);
    else M5.Display.print("Humidity: --.- %   ");
    M5.Display.setCursor(8, 108);
    if (env.valid) M5.Display.printf("Pressure: %.1f hPa  ", env.pressurePa / 100.0f);
    else M5.Display.print("Pressure: ---- hPa  ");
    M5.Display.setCursor(8, 144);
    M5.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5.Display.printf("Cooling power: %3.0f %%  ", state.coolingPower * 100.0f);
    M5.Display.setCursor(8, 184);
    M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5.Display.printf("Target: %.1f C         ", DEFAULT_TARGET_C);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);

  } else if (mode == UiMode::SET_TEMPERATURE) {
    M5.Display.setCursor(8, 58);
    M5.Display.printf("Target: %.1f C   ", targetC);
    M5.Display.setCursor(8, 100);
    M5.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5.Display.println("A: warmer");
    M5.Display.setCursor(8, 130);
    M5.Display.println("B: cooler");
    M5.Display.setCursor(8, 160);
    M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5.Display.println("C: confirm");

  } else if (mode == UiMode::SET_DURATION) {
    char duration[16];
    printDuration(selectedDurationSeconds, duration, sizeof(duration));
    M5.Display.setCursor(8, 58);
    M5.Display.printf("Target: %.1f C   ", targetC);
    M5.Display.setCursor(8, 94);
    M5.Display.printf("Duration: %s    ", duration);
    M5.Display.setCursor(8, 132);
    M5.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5.Display.println("A: +10 min");
    M5.Display.setCursor(8, 160);
    M5.Display.println("B: -10 min");
    M5.Display.setCursor(8, 188);
    M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5.Display.println("C: start cooling");

  } else if (mode == UiMode::COOLING) {
    char remaining[16];
    const uint32_t hoursLeft = remainingSeconds / 3600UL;
    const uint32_t minutesLeft = (remainingSeconds % 3600UL) / 60UL;
    const uint32_t secondsLeft = remainingSeconds % 60UL;
    snprintf(remaining, sizeof(remaining), "%02lu:%02lu:%02lu",
             (unsigned long)hoursLeft, (unsigned long)minutesLeft,
             (unsigned long)secondsLeft);
    M5.Display.setCursor(8, 56);
    M5.Display.printf("Target: %.1f C   ", targetC);
    M5.Display.setCursor(8, 88);
    M5.Display.print("Current: ");
    if (env.valid) M5.Display.printf("%.1f C     ", env.temperatureC);
    else M5.Display.print("--.- C     ");
    M5.Display.setCursor(8, 120);
    M5.Display.printf("Remaining: %s   ", remaining);
    M5.Display.setCursor(8, 152);
    M5.Display.printf("Cooling: %3.0f %%   ", state.coolingPower * 100.0f);
    M5.Display.setCursor(8, 184);
    M5.Display.setTextColor(_sensorInitOk && env.valid ? TFT_GREEN : TFT_RED, TFT_BLACK);
    M5.Display.printf("ENV III: %s       ", (_sensorInitOk && env.valid) ? "OK" : "CHECK");

  } else if (mode == UiMode::STOP_CONFIRM) {
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setCursor(8, 58);
    M5.Display.println("Do you want to stop");
    M5.Display.setCursor(8, 84);
    M5.Display.println("custom configuration?");
    M5.Display.setCursor(8, 124);
    M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5.Display.printf("A: YES");
    M5.Display.setCursor(8, 154);
    M5.Display.setTextColor(TFT_RED, TFT_BLACK);
    M5.Display.printf("B: NO");

  } else {
    M5.Display.setCursor(8, 72);
    M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5.Display.println("Timer finished.");
    M5.Display.setCursor(8, 116);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.println("Cooling is off.");
    M5.Display.setCursor(8, 164);
    M5.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5.Display.println("Press any button");
  }

  M5.Display.setTextSize(1);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setCursor(8, M5.Display.height() - 13);
  if (mode == UiMode::MONITOR) M5.Display.print("Press any button to configure. ");
  else if (mode == UiMode::COOLING) M5.Display.print("C: stop custom configuration   ");
  else if (mode == UiMode::STOP_CONFIRM) M5.Display.print("A: YES   B: NO    ");
  else if (mode == UiMode::COMPLETE) M5.Display.print("Press any button to return.    ");
  else M5.Display.print("M5Stack Fire buttons A / B / C ");
  M5.Display.setTextSize(2);
}
