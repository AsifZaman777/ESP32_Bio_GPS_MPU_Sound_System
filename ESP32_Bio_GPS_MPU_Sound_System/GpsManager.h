#ifndef GPS_MANAGER_H
#define GPS_MANAGER_H

#include <Arduino.h>
#include <TinyGPS++.h>
#include "ProjectConfig.h"

struct GpsData {
  bool locationValid = false;
  double latitude = 0;
  double longitude = 0;
  uint32_t satellites = 0;
  double speedKmph = 0;
  uint32_t charsProcessed = 0;
};

class GpsManager {
public:
  void begin();
  void update();
  GpsData getData() const;

private:
  TinyGPSPlus gps;
  GpsData data;
};

#endif
