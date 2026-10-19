# ClimateSimulator — M5Stack Fire

Object-oriented air-conditioner simulator for Arduino IDE.

## Libraries
Install through Arduino IDE Library Manager:
- M5Unified
- M5Unit-ENV (plus dependencies prompted by the Library Manager)
- Adafruit NeoPixel

Recommended initial target: **M5Stack Arduino core / ESP32 board package 2.0.17** because this example uses the core-2.x LEDC API (`ledcSetup`, `ledcAttachPin`, `ledcWrite(channel, duty)`).

## Pin map — verify before wiring
- ENV III: SDA GPIO21, SCL GPIO22, 5 V, GND (PORT A)
- Unit Angle: AOUT / white wire to GPIO36 (PORT B); red 5 V, black GND. Verify the AOUT maximum voltage is safe for ESP32 ADC (3.3 V max).
- UART2: M5Fire GPIO16 = RX (connect to remote TX); GPIO17 = TX (connect to remote RX); common GND; 115200 8N1.
- LED data: GPIO13 to WS2812B DIN through a 330–470 ohm series resistor; common ground. Use suitable 5 V supply and level shifter if required. Do not power a high-current LED load from an ESP32 GPIO.
- Fan PWM: GPIO26 only to the input/gate driver of an external logic-level N-MOSFET module. Fan red -> +5 V supply, black -> MOSFET drain, MOSFET source -> GND. Grounds must be common. Add appropriate flyback protection if required by the fan/driver. Do not connect fan power to a GPIO.

GPIO access depends on which M5Fire connector/base is used. Check the M5Stack Fire pin map and avoid pins used by LCD, SD card, speaker, or other onboard devices.

## Fan safety
Waveshare Fan-4010-5V is a 5 V, 3-pin fan. The third wire is commonly a tachometer output, not a PWM input. The code assumes low-side power PWM through an external MOSFET. This may not give smooth speed control on every fan; test at low duty and set `FAN_MIN_START_PERCENT` to a value that reliably starts the fan. If uncertain, use simple ON/OFF control first. Never power the fan from an ESP32 GPIO.

## Local / remote mode
- Press M5Stack button A to toggle LOCAL POT / REMOTE UART.
- Local mode maps the potentiometer to 16–30 °C.
- Remote mode uses the most recent valid `SET,<temperature>` command.
- If remote mode is selected but no valid remote setting has arrived in the last 5 seconds, cooling output is forced to zero until a fresh valid `SET` arrives. The screen/telemetry shows remote freshness.

## UART protocol
Each message is ASCII terminated by LF (`\n`), 115200 baud, 8N1.

Commands from remote ESP32 to M5Fire:
- `SET,22.5` — set remote target in °C, valid range 16.0–30.0
- `MODE,LOCAL` — select local potentiometer
- `MODE,REMOTE` — select remote target
- `PING` — response `PONG`

Replies / telemetry from M5Fire:
- `ACK,SET`
- `ACK,MODE`
- `ERR,SET_RANGE`
- `ERR,UNKNOWN_COMMAND`
- `ENV,<tempC>,<humidityPct>,<pressurePa>,<targetC>,<power0to1>,<LOCAL|REMOTE>`

Example: `ENV,25.31,44.10,100820,22.50,0.471,REMOTE`

## Notes
- SHT30 and QMP6988 are read once per second.
- Invalid sensor readings cause cooling power to go to zero.
- The pressure value from the library is in Pa; the screen displays hPa.
- The fan is a simulated cooling actuator; it does not actively cool the room.
