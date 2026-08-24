#include "GpsManager.h"

void GpsManager::begin() {
  Serial.println("Initializing NEO-M8N GPS...");
  Serial2.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("GPS UART2 OK. Waiting for satellite fix.");
}

void GpsManager::update() {
  while (Serial2.available() > 0) {
    gps.encode(Serial2.read());
  }

  data.charsProcessed = gps.charsProcessed();

  if (gps.location.isValid()) {
    data.locationValid = true;
    data.latitude = gps.location.lat();
    data.longitude = gps.location.lng();
  } else {
    data.locationValid = false;
  }

  if (gps.satellites.isValid()) {
    data.satellites = gps.satellites.value();
  }

  if (gps.speed.isValid()) {
    data.speedKmph = gps.speed.kmph();
  }
}

GpsData GpsManager::getData() const {
  return data;
}
