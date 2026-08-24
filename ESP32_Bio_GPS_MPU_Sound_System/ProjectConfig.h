#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include <Arduino.h>

// Serial monitor
const uint32_t SERIAL_BAUD = 115200;

// Shared I2C bus: MAX30102 and MPU6050
const uint8_t I2C_SDA_PIN = 21;
const uint8_t I2C_SCL_PIN = 22;
const uint32_t I2C_CLOCK_HZ = 100000; // More reliable with mixed/clone I2C modules

// NEO-M8N GPS on ESP32 UART2
const uint32_t GPS_BAUD = 9600;
const uint8_t GPS_RX_PIN = 16; // Connect GPS TX to ESP32 GPIO16
const uint8_t GPS_TX_PIN = 17; // Connect GPS RX to ESP32 GPIO17

// Analog sound sensor
const uint8_t SOUND_ADC_PIN = 34; // Input-only ADC pin
const uint16_t SOUND_VOICE_THRESHOLD = 180;

// Scheduler intervals
const unsigned long MAX30102_INTERVAL_MS = 10; // 100 Hz
const unsigned long MPU_INTERVAL_MS = 20;      // 50 Hz
const unsigned long SOUND_INTERVAL_MS = 5;     // 200 Hz
const unsigned long PRINT_INTERVAL_MS = 1000;  // 1 Hz readable grouped output

// MAX30102 tuning
const long FINGER_THRESHOLD = 50000;
const unsigned long BEAT_TIMEOUT_MS = 3000;

#endif
