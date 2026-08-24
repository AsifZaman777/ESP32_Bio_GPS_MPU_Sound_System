#ifndef MPU6050_MANAGER_H
#define MPU6050_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include "ProjectConfig.h"

struct Mpu6050Data {
  float ax = 0;
  float ay = 0;
  float az = 0;
  float gx = 0;
  float gy = 0;
  float gz = 0;
  float temperatureC = 0;
  uint8_t whoAmI = 0;
  bool sensorOk = false;
};

class Mpu6050Manager {
public:
  void begin();
  void update(unsigned long now);
  Mpu6050Data getData() const;

private:
  bool writeRegister(uint8_t reg, uint8_t value);
  bool readRegisters(uint8_t reg, uint8_t *buffer, uint8_t length);
  int16_t makeInt16(uint8_t highByte, uint8_t lowByte);

  Mpu6050Data data;
  unsigned long lastSampleMs = 0;
};

#endif
