#include <Wire.h>
#include "ProjectConfig.h"
#include "Max30102Manager.h"
#include "GpsManager.h"
#include "Mpu6050Manager.h"
#include "SoundSensorManager.h"

Max30102Manager max30102;
GpsManager gps;
Mpu6050Manager mpu;
SoundSensorManager soundSensor;

unsigned long lastPrintMs = 0;

const char *max30102SignalText(uint8_t quality) {
  switch (quality) {
    case 0:
      return "NO_FINGER";
    case 1:
      return "HIGH_DC_REDUCE_LED_OR_PRESSURE";
    case 2:
      return "WEAK_WAVE_ADJUST_FINGER";
    case 3:
      return "NOISY_KEEP_STILL";
    case 4:
      return "GOOD";
    default:
      return "UNKNOWN";
  }
}

void printSystemHeader() {
  Serial.println();
  Serial.println("ESP32 Bio + GPS + MPU6050 + Sound Sensor System");
  Serial.println("Sensors:");
  Serial.println("- MAX30102 on I2C");
  Serial.println("- MPU6050 on I2C");
  Serial.println("- NEO-M8N GPS on UART2");
  Serial.println("- Analog sound sensor on ADC");
  Serial.println();
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1000);

  printSystemHeader();

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(I2C_CLOCK_HZ);

  max30102.begin();
  mpu.begin();
  gps.begin();
  soundSensor.begin();

  Serial.println();
  Serial.println("System init finished.");
  Serial.println("Readable grouped Serial Monitor output enabled.");
  Serial.println();
}

void printGroupedData(unsigned long now) {
  Max30102Data bio = max30102.getData();
  GpsData gpsData = gps.getData();
  Mpu6050Data motion = mpu.getData();
  SoundData audio = soundSensor.getData();

  Serial.println("==================================================");
  Serial.print("TIME: ");
  Serial.print(now);
  Serial.println(" ms");

  Serial.println("[MAX30102 - HEART / SpO2 RAW]");
  Serial.print("Sensor Status : ");
  Serial.println(bio.sensorOk ? "OK" : "NOT FOUND");
  Serial.print("Finger        : ");
  Serial.println(bio.fingerDetected ? "DETECTED" : "NOT DETECTED");
  Serial.print("IR Raw        : ");
  Serial.println(bio.ir);
  Serial.print("IR Wave AC    : ");
  Serial.println(bio.irAC);
  Serial.print("AC Envelope   : ");
  Serial.println(bio.acEnvelope);
  Serial.print("Signal Quality: ");
  Serial.println(max30102SignalText(bio.signalQuality));
  Serial.print("Beat          : ");
  if (bio.beatDetected && bio.fallbackBeat) {
    Serial.println("YES - AC FALLBACK");
  } else {
    Serial.println(bio.beatDetected ? "YES" : "NO");
  }
  Serial.print("BPM           : ");
  Serial.println(bio.bpm, 1);
  Serial.print("Average BPM   : ");
  Serial.println(bio.avgBpm);
  Serial.println();

  Serial.println("[GPS - NEO-M8N]");
  Serial.print("Location Fix  : ");
  Serial.println(gpsData.locationValid ? "VALID" : "NO FIX");
  Serial.print("Latitude      : ");
  Serial.println(gpsData.latitude, 6);
  Serial.print("Longitude     : ");
  Serial.println(gpsData.longitude, 6);
  Serial.print("Satellites    : ");
  Serial.println(gpsData.satellites);
  Serial.print("Speed         : ");
  Serial.print(gpsData.speedKmph, 2);
  Serial.println(" km/h");
  Serial.print("NMEA Chars    : ");
  Serial.println(gpsData.charsProcessed);
  Serial.println();

  Serial.println("[MPU6050 - GYRO / ACCEL]");
  Serial.print("Sensor Status : ");
  Serial.println(motion.sensorOk ? "OK" : "NOT FOUND");
  Serial.print("WHO_AM_I      : 0x");
  Serial.println(motion.whoAmI, HEX);
  Serial.print("Accel X       : ");
  Serial.print(motion.ax, 3);
  Serial.println(" m/s^2");
  Serial.print("Accel Y       : ");
  Serial.print(motion.ay, 3);
  Serial.println(" m/s^2");
  Serial.print("Accel Z       : ");
  Serial.print(motion.az, 3);
  Serial.println(" m/s^2");
  Serial.print("Gyro X        : ");
  Serial.print(motion.gx, 3);
  Serial.println(" rad/s");
  Serial.print("Gyro Y        : ");
  Serial.print(motion.gy, 3);
  Serial.println(" rad/s");
  Serial.print("Gyro Z        : ");
  Serial.print(motion.gz, 3);
  Serial.println(" rad/s");
  Serial.print("Temperature   : ");
  Serial.print(motion.temperatureC, 2);
  Serial.println(" C");
  Serial.println();

  Serial.println("[SOUND SENSOR - ANALOG VOICE LEVEL]");
  Serial.print("ADC Raw       : ");
  Serial.println(audio.raw);
  Serial.print("ADC Baseline  : ");
  Serial.println(audio.baseline);
  Serial.print("Sound Envelope: ");
  Serial.println(audio.envelope);
  Serial.print("Voice/Noise   : ");
  Serial.println(audio.voiceDetected ? "DETECTED" : "LOW");
  Serial.println("==================================================");
  Serial.println();
}

void loop() {
  unsigned long now = millis();

  max30102.update(now);
  gps.update();
  mpu.update(now);
  soundSensor.update(now);

  if (now - lastPrintMs >= PRINT_INTERVAL_MS) {
    lastPrintMs = now;
    printGroupedData(now);
  }
}
