#ifndef SOUND_SENSOR_MANAGER_H
#define SOUND_SENSOR_MANAGER_H

#include <Arduino.h>
#include "ProjectConfig.h"

struct SoundData {
  int raw = 0;
  int baseline = 2048;
  int envelope = 0;
  bool voiceDetected = false;
};

class SoundSensorManager {
public:
  void begin();
  void update(unsigned long now);
  SoundData getData() const;

private:
  SoundData data;
  unsigned long lastSampleMs = 0;
};

#endif
