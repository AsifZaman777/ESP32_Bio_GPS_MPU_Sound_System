#include "Mpu6050Manager.h"

const uint8_t MPU6050_ADDRESS = 0x68;
const uint8_t MPU6050_REG_ACCEL_XOUT_H = 0x3B;
const uint8_t MPU6050_REG_PWR_MGMT_1 = 0x6B;
const uint8_t MPU6050_REG_SMPLRT_DIV = 0x19;
const uint8_t MPU6050_REG_CONFIG = 0x1A;
const uint8_t MPU6050_REG_GYRO_CONFIG = 0x1B;
const uint8_t MPU6050_REG_ACCEL_CONFIG = 0x1C;
const uint8_t MPU6050_REG_WHO_AM_I = 0x75;

void Mpu6050Manager::begin() {
  Serial.println("Initializing MPU6050...");

  uint8_t who = 0;
  if (!readRegisters(MPU6050_REG_WHO_AM_I, &who, 1)) {
    Serial.println("MPU6050 ERROR: Address 0x68 found by scanner, but register read failed.");
    Serial.println("Check power stability, jumper wires, and shared I2C pull-ups.");
    data.sensorOk = false;
    return;
  }

  data.whoAmI = who;
  Serial.print("MPU6050 WHO_AM_I: 0x");
  Serial.println(who, HEX);

  if (who != 0x68 && who != 0x69 && who != 0x70) {
    Serial.println("MPU6050 WARNING: Unexpected WHO_AM_I value.");
    Serial.println("Trying direct register mode anyway.");
  }

  // Wake the sensor and configure practical ranges for motion monitoring
  bool ok = true;
  ok &= writeRegister(MPU6050_REG_PWR_MGMT_1, 0x00);   // Wake up
  delay(100);
  ok &= writeRegister(MPU6050_REG_SMPLRT_DIV, 0x07);   // 1 kHz / (7 + 1) = 125 Hz
  ok &= writeRegister(MPU6050_REG_CONFIG, 0x03);       // DLPF around 44 Hz
  ok &= writeRegister(MPU6050_REG_GYRO_CONFIG, 0x08);  // +/- 500 deg/s
  ok &= writeRegister(MPU6050_REG_ACCEL_CONFIG, 0x10); // +/- 8 g

  if (!ok) {
    Serial.println("MPU6050 ERROR: Configuration write failed.");
    data.sensorOk = false;
    return;
  }

  data.sensorOk = true;
  Serial.println("MPU6050 OK in direct register mode.");
}

void Mpu6050Manager::update(unsigned long now) {
  if (!data.sensorOk) {
    return;
  }

  if (now - lastSampleMs < MPU_INTERVAL_MS) {
    return;
  }
  lastSampleMs = now;

  uint8_t raw[14];
  if (!readRegisters(MPU6050_REG_ACCEL_XOUT_H, raw, 14)) {
    data.sensorOk = false;
    Serial.println("MPU6050 ERROR: Runtime read failed.");
    return;
  }

  int16_t accelX = makeInt16(raw[0], raw[1]);
  int16_t accelY = makeInt16(raw[2], raw[3]);
  int16_t accelZ = makeInt16(raw[4], raw[5]);
  int16_t tempRaw = makeInt16(raw[6], raw[7]);
  int16_t gyroX = makeInt16(raw[8], raw[9]);
  int16_t gyroY = makeInt16(raw[10], raw[11]);
  int16_t gyroZ = makeInt16(raw[12], raw[13]);

  // +/-8 g range: 4096 LSB/g, convert g to m/s^2
  data.ax = (accelX / 4096.0) * 9.80665;
  data.ay = (accelY / 4096.0) * 9.80665;
  data.az = (accelZ / 4096.0) * 9.80665;

  // +/-500 deg/s range: 65.5 LSB/(deg/s), convert deg/s to rad/s
  data.gx = (gyroX / 65.5) * DEG_TO_RAD;
  data.gy = (gyroY / 65.5) * DEG_TO_RAD;
  data.gz = (gyroZ / 65.5) * DEG_TO_RAD;

  data.temperatureC = (tempRaw / 340.0) + 36.53;
}

Mpu6050Data Mpu6050Manager::getData() const {
  return data;
}

bool Mpu6050Manager::writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool Mpu6050Manager::readRegisters(uint8_t reg, uint8_t *buffer, uint8_t length) {
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  uint8_t received = Wire.requestFrom(MPU6050_ADDRESS, length);
  if (received != length) {
    return false;
  }

  for (uint8_t i = 0; i < length; i++) {
    buffer[i] = Wire.read();
  }

  return true;
}

int16_t Mpu6050Manager::makeInt16(uint8_t highByte, uint8_t lowByte) {
  return (int16_t)((highByte << 8) | lowByte);
}
