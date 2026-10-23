#pragma once
#include <Arduino.h>

// M5Stack Fire with ENV III connected to PORT A.
static constexpr int PIN_I2C_SDA = 21;
static constexpr int PIN_I2C_SCL = 22;

// Optional WS2812/NeoPixel ring output. Verify your actual data pin.
static constexpr int PIN_LED_DATA = 13;
static constexpr uint16_t LED_COUNT = 8;
static constexpr uint8_t LED_BRIGHTNESS = 80;

static constexpr float MIN_TARGET_C = 16.0f;
static constexpr float MAX_TARGET_C = 30.0f;
static constexpr float DEFAULT_TARGET_C = 22.0f;
static constexpr float TARGET_STEP_C = 0.5f;
static constexpr uint32_t DURATION_STEP_SECONDS = 10UL * 60UL;
static constexpr uint32_t MIN_DURATION_SECONDS = 10UL * 60UL;
static constexpr uint32_t MAX_DURATION_SECONDS = 24UL * 60UL * 60UL;
static constexpr float FULL_COOLING_DELTA_C = 6.0f;

static constexpr uint32_t SENSOR_INTERVAL_MS = 1000;
static constexpr uint32_t CONTROL_INTERVAL_MS = 100;
static constexpr uint32_t DISPLAY_INTERVAL_MS = 500;
