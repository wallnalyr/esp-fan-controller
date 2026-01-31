# ESP32-C3 Fan Controller

Temperature-based PWM fan controller using an ESP32-C3 and DS18B20 sensor.

## Features

- Reads temperature from DS18B20 sensor
- Controls Noctua PWM fan speed based on temperature
- Serial output for monitoring

## Temperature Response

| Temperature | Fan Speed |
|-------------|-----------|
| Below 80°F  | 30% (minimum) |
| 80°F - 85°F | 30% - 100% (linear ramp) |
| Above 85°F  | 100% |

## Hardware

### Components

- ESP32-C3 development board
- DS18B20 temperature sensor
- Noctua 4-pin PWM fan
- 4.7kΩ resistor (pull-up for DS18B20)

### Wiring

```
ESP32-C3          DS18B20
---------         -------
3.3V  ──────────── VDD (red)
GND   ──────────── GND (black)
GPIO4 ──────┬───── DATA (yellow)
            │
           4.7kΩ
            │
3.3V  ──────┘

ESP32-C3          Noctua Fan
---------         ----------
GPIO8 ──────────── PWM (blue)
GND   ──────────── GND (black)
                   +12V (yellow) ── 12V supply
                   Tach (green) ── (optional, not used)
```

### Pin Configuration

| GPIO | Function |
|------|----------|
| 4    | DS18B20 data |
| 8    | Fan PWM output |

## Software Setup

### Arduino IDE

1. Install ESP32 board support:
   - Go to **File > Preferences**
   - Add to Board Manager URLs:
     ```
     https://espressif.github.io/arduino-esp32/package_esp32_index.json
     ```
   - Go to **Tools > Board > Boards Manager**
   - Search for "esp32" and install

2. Select board:
   - **Tools > Board > esp32 > ESP32C3 Dev Module**

3. Install required libraries via **Tools > Manage Libraries**:
   - `OneWire` by Jim Studt
   - `DallasTemperature` by Miles Burton

4. Open `esp-fan-controller.ino` and upload

### Serial Monitor

Set baud rate to **115200** to view temperature readings and fan status.

## Configuration

Edit these constants in the sketch to adjust behavior:

```cpp
#define TEMP_MIN 80.0f    // Temperature to start ramping (°F)
#define TEMP_MAX 85.0f    // Temperature for full speed (°F)
#define FAN_MIN_DUTY 77   // Minimum duty cycle (~30%)
#define FAN_MAX_DUTY 255  // Maximum duty cycle (100%)
```

## Notes

- Noctua fans require 25kHz PWM signal (configured in sketch)
- Minimum 30% duty cycle prevents fan stall on Noctua fans
- DS18B20 readings occur every 2 seconds
- Ensure adequate 12V power supply for the fan
