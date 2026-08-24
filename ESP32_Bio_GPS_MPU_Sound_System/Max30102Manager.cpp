#include "Max30102Manager.h"

void Max30102Manager::begin() {
  Serial.println("Initializing MAX30102...");

  if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 ERROR: Not found at I2C address 0x57.");
    Serial.println("Check sensor power, GND, SDA=21, SCL=22, and pull-ups.");
    data.sensorOk = false;
    return;
  }

  // Moderate LED current prevents flat saturated readings on many modules
  byte ledBrightness = 0x1F; // Lower current helps avoid high-DC flat readings
  byte sampleAverage = 4;
  byte ledMode = 2;          // Red + IR
  int sampleRate = 100;
  int pulseWidth = 411;      // 18-bit resolution
  int adcRange = 8192;       // More headroom when finger DC level is high

  sensor.setup(
    ledBrightness,
    sampleAverage,
    ledMode,
    sampleRate,
    pulseWidth,
    adcRange
  );

  sensor.setPulseAmplitudeRed(0x0F);
  sensor.setPulseAmplitudeIR(0x1F);
  sensor.setPulseAmplitudeGreen(0);
  sensor.clearFIFO();

  resetBeatData();
  data.sensorOk = true;
  Serial.println("MAX30102 OK.");
}

void Max30102Manager::update(unsigned long now) {
  if (!data.sensorOk) {
    return;
  }

  if (now - lastSampleMs < MAX30102_INTERVAL_MS) {
    return;
  }
  lastSampleMs = now;

  data.ir = sensor.getIR();
  irDC = dcAlpha * irDC + (1.0 - dcAlpha) * data.ir;
  data.irAC = data.ir - (long)irDC;
  data.fingerDetected = data.ir > FINGER_THRESHOLD;
  data.beatDetected = false;
  data.fallbackBeat = false;

  int absAC = abs(data.irAC);
  if (absAC > data.acEnvelope) {
    data.acEnvelope = absAC;
  } else {
    data.acEnvelope = (data.acEnvelope * 95) / 100;
  }

  if (!data.fingerDetected) {
    data.signalQuality = 0; // No finger
  } else if (data.ir > 210000) {
    data.signalQuality = 1; // Too much DC light
  } else if (data.acEnvelope < 250) {
    data.signalQuality = 2; // Weak pulse waveform
  } else if (data.acEnvelope > 4000) {
    data.signalQuality = 3; // Too much motion/noise
  } else {
    data.signalQuality = 4; // Usable signal
  }

  if (!data.fingerDetected) {
    resetBeatData();
    return;
  }

  if (checkForBeat(data.ir)) {
    acceptBeat(now);
  } else if (detectBeatFromAC(now)) {
    data.fallbackBeat = true;
    acceptBeat(now);
  }

  if (lastValidBeatTime > 0 && now - lastValidBeatTime > BEAT_TIMEOUT_MS) {
    data.bpm = 0;
    data.avgBpm = 0;
  }
}

Max30102Data Max30102Manager::getData() const {
  return data;
}

void Max30102Manager::resetBeatData() {
  data.bpm = 0;
  data.avgBpm = 0;
  data.beatDetected = false;
  data.fallbackBeat = false;
  data.acEnvelope = 0;
  rateSpot = 0;
  lastBeat = 0;
  lastValidBeatTime = 0;
  lastFallbackBeatMs = 0;
  previousIrAC = 0;

  for (byte i = 0; i < 8; i++) {
    rates[i] = 0;
  }
}

void Max30102Manager::acceptBeat(unsigned long now) {
  data.beatDetected = true;

  if (lastBeat > 0) {
    unsigned long delta = now - lastBeat;
    data.bpm = 60.0 / (delta / 1000.0);

    if (data.bpm > 40 && data.bpm < 180) {
      rates[rateSpot++] = (byte)data.bpm;
      rateSpot %= 8;

      int sum = 0;
      byte validCount = 0;

      for (byte i = 0; i < 8; i++) {
        if (rates[i] > 0) {
          sum += rates[i];
          validCount++;
        }
      }

      if (validCount > 0) {
        data.avgBpm = sum / validCount;
      }

      lastValidBeatTime = now;
    }
  }

  lastBeat = now;
}

bool Max30102Manager::detectBeatFromAC(unsigned long now) {
  if (data.acEnvelope < 250) {
    previousIrAC = data.irAC;
    return false;
  }

  int threshold = data.acEnvelope / 2;
  if (threshold < 180) {
    threshold = 180;
  }

  bool risingCross = previousIrAC <= threshold && data.irAC > threshold;
  bool refractoryOk = now - lastFallbackBeatMs > 350;
  bool maxGapOk = now - lastFallbackBeatMs < 2000 || lastFallbackBeatMs == 0;

  previousIrAC = data.irAC;

  if (risingCross && refractoryOk && maxGapOk) {
    lastFallbackBeatMs = now;
    return true;
  }

  return false;
}
