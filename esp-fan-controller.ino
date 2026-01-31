/*
 * ESP32-C3 Fan Controller
 *
 * Controls a Noctua PWM fan based on DS18B20 temperature readings.
 * - Below 80°F: 30% fan speed (Noctua minimum)
 * - 80-85°F: Linear ramp from 30% to 100%
 * - Above 85°F: 100% fan speed
 */

#include <OneWire.h>
#include <DallasTemperature.h>

// Pin definitions
#define DS18B20_PIN 4   // GPIO4 for DS18B20 data (needs 4.7kΩ pull-up)
#define FAN_PWM_PIN 8   // GPIO8 for fan PWM output

// PWM configuration for Noctua fans (25kHz)
#define PWM_FREQ 25000
#define PWM_RESOLUTION 8  // 8-bit (0-255)
#define PWM_CHANNEL 0

// Temperature thresholds (Fahrenheit)
#define TEMP_MIN 80.0f
#define TEMP_MAX 85.0f

// Fan speed limits (duty cycle 0-255)
#define FAN_MIN_DUTY 77   // ~30% of 255
#define FAN_MAX_DUTY 255  // 100%

// Timing
#define READ_INTERVAL_MS 2000

// OneWire and DallasTemperature instances
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

unsigned long lastReadTime = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000); // Wait for serial (max 3s)

  Serial.println();
  Serial.println("ESP32-C3 Fan Controller");
  Serial.println("=======================");

  // Initialize DS18B20
  sensors.begin();
  int deviceCount = sensors.getDeviceCount();
  Serial.print("Found ");
  Serial.print(deviceCount);
  Serial.println(" DS18B20 sensor(s)");

  if (deviceCount == 0) {
    Serial.println("WARNING: No sensors found! Check wiring.");
  }

  // Configure PWM using LEDC
  ledcAttach(FAN_PWM_PIN, PWM_FREQ, PWM_RESOLUTION);

  // Start at minimum speed
  ledcWrite(FAN_PWM_PIN, FAN_MIN_DUTY);
  Serial.println("Fan initialized at 30% speed");
  Serial.println();
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastReadTime >= READ_INTERVAL_MS) {
    lastReadTime = currentTime;

    // Request temperature reading
    sensors.requestTemperatures();
    float tempF = sensors.getTempFByIndex(0);

    // Check for valid reading
    if (tempF == DEVICE_DISCONNECTED_F) {
      Serial.println("Error: Sensor disconnected!");
      return;
    }

    // Calculate fan duty cycle
    uint8_t duty = calculateFanDuty(tempF);

    // Apply PWM
    ledcWrite(FAN_PWM_PIN, duty);

    // Debug output
    int percent = map(duty, 0, 255, 0, 100);
    Serial.print("Temp: ");
    Serial.print(tempF, 1);
    Serial.print("°F | Fan: ");
    Serial.print(percent);
    Serial.print("% (duty: ");
    Serial.print(duty);
    Serial.println(")");
  }
}

uint8_t calculateFanDuty(float tempF) {
  if (tempF < TEMP_MIN) {
    // Below 80°F: minimum speed
    return FAN_MIN_DUTY;
  } else if (tempF >= TEMP_MAX) {
    // Above 85°F: maximum speed
    return FAN_MAX_DUTY;
  } else {
    // Linear interpolation between 80-85°F
    float ratio = (tempF - TEMP_MIN) / (TEMP_MAX - TEMP_MIN);
    return FAN_MIN_DUTY + (uint8_t)(ratio * (FAN_MAX_DUTY - FAN_MIN_DUTY));
  }
}
