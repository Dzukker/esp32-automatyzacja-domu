#pragma once
#include <Arduino.h>

// M5Stack Fire / M5GO base assumptions. VERIFY physical connector mapping.
static constexpr int PIN_I2C_SDA = 21;      // PORT A / ENV III
static constexpr int PIN_I2C_SCL = 22;      // PORT A / ENV III
static constexpr int PIN_POT_ADC = 36;      // PORT B white wire = AOUT (input-only ADC1)
static constexpr int PIN_LED_DATA = 13;     // M-Bus pin; verify access on your setup
static constexpr int PIN_FAN_PWM = 26;      // external MOSFET gate driver input
static constexpr int PIN_UART_RX = 16;      // ESP32 UART2 RX <- remote ESP32 TX
static constexpr int PIN_UART_TX = 17;      // ESP32 UART2 TX -> remote ESP32 RX

static constexpr uint32_t UART_BAUD = 115200;
static constexpr uint16_t LED_COUNT = 8;
static constexpr uint8_t LED_BRIGHTNESS = 80;

static constexpr float MIN_TARGET_C = 16.0f;
static constexpr float MAX_TARGET_C = 30.0f;
static constexpr float DEFAULT_TARGET_C = 22.0f;
static constexpr float FULL_COOLING_DELTA_C = 6.0f;

static constexpr uint32_t SENSOR_INTERVAL_MS = 1000;
static constexpr uint32_t CONTROL_INTERVAL_MS = 100;
static constexpr uint32_t DISPLAY_INTERVAL_MS = 500;
static constexpr uint32_t TELEMETRY_INTERVAL_MS = 1000;
static constexpr uint32_t REMOTE_TIMEOUT_MS = 5000;

// PWM settings for an external low-side MOSFET switching the 5 V fan.
// Fan-4010-5V is a 3-wire 5 V fan; its third wire is typically tachometer,
// not PWM control. Do not connect the fan directly to an ESP32 GPIO.
static constexpr uint32_t FAN_PWM_HZ = 25000;
static constexpr uint8_t FAN_PWM_BITS = 8;
static constexpr uint8_t FAN_MIN_START_PERCENT = 35; // compensate for stall at low duty
