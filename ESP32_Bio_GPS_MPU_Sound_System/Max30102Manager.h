#ifndef MAX30102_MANAGER_H
#define MAX30102_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "ProjectConfig.h"

struct Max30102Data {
  long ir = 0;
  long irAC = 0;
  bool fingerDetected = false;
  bool beatDetected = false;
  bool fallbackBeat = false;
  float bpm = 0;
  int avgBpm = 0;
  int acEnvelope = 0;
  uint8_t signalQuality = 0;
  bool sensorOk = false;
};

class Max30102Manager {
public:
  void begin();
  void update(unsigned long now);
  Max30102Data getData() const;

private:
  void resetBeatData();
  void acceptBeat(unsigned long now);
  bool detectBeatFromAC(unsigned long now);

  MAX30105 sensor;
  Max30102Data data;

  byte rates[8] = {0};
  byte rateSpot = 0;
  unsigned long lastBeat = 0;
  unsigned long lastValidBeatTime = 0;
  unsigned long lastSampleMs = 0;
  unsigned long lastFallbackBeatMs = 0;

  float irDC = 0;
  const float dcAlpha = 0.95;
  long previousIrAC = 0;
};

#endif
