# ESP32 Bio + GPS + MPU6050 + Sound Sensor System

## Pin Map

| Module | ESP32 Pin |
|---|---|
| MAX30102 SDA | GPIO21 |
| MAX30102 SCL | GPIO22 |
| MPU6050 SDA | GPIO21 |
| MPU6050 SCL | GPIO22 |
| GPS TX | GPIO16 RX2 |
| GPS RX | GPIO17 TX2 |
| Sound analog OUT | GPIO34 |

## Required Arduino Libraries

- SparkFun MAX3010x Sensor Library
- TinyGPSPlus

MPU6050 direct register mode ব্যবহার করা হয়েছে, তাই MPU6050-এর জন্য আলাদা Adafruit library বাধ্যতামূলক নয়।

## Serial Output

Baud rate: `115200`

Serial Monitor output grouped format:

- MAX30102 heart sensor data
- GPS NEO-M8N location data
- MPU6050 gyro/accelerometer data
- Analog sound sensor voice/noise level

## Hardware Notes

- MAX30102 and MPU6050 share the same I2C bus.
- NEO-M8N GPS first fix can take several minutes outdoors.
- GPS TX must connect to ESP32 RX2 GPIO16.
- GPS RX must connect to ESP32 TX2 GPIO17.
- Use GPIO34 for analog sound input because it is ADC input-only and safe for sensor input.
- If MAX30102 IR waveform is flat, reduce IR LED current in `Max30102Manager.cpp`.
- MAX30102 output now includes signal quality and an AC waveform fallback beat detector.
