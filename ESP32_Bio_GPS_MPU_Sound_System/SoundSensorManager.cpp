#include "SoundSensorManager.h"

void SoundSensorManager::begin() {
  Serial.println("Initializing analog sound sensor...");
  analogReadResolution(12);
  analogSetPinAttenuation(SOUND_ADC_PIN, ADC_11db);

  long sum = 0;
  for (int i = 0; i < 100; i++) {
    sum += analogRead(SOUND_ADC_PIN);
    delay(2);
  }

  data.baseline = sum / 100;
  Serial.print("Sound sensor baseline: ");
  Serial.println(data.baseline);
}

void SoundSensorManager::update(unsigned long now) {
  if (now - lastSampleMs < SOUND_INTERVAL_MS) {
    return;
  }
  lastSampleMs = now;

  data.raw = analogRead(SOUND_ADC_PIN);

  // Slow baseline tracking removes DC offset from analog microphone modules
  data.baseline = (data.baseline * 99 + data.raw) / 100;
  int amplitude = abs(data.raw - data.baseline);

  // Envelope detector smooths the audio amplitude for voice/noise detection
  if (amplitude > data.envelope) {
    data.envelope = amplitude;
  } else {
    data.envelope = (data.envelope * 9) / 10;
  }

  data.voiceDetected = data.envelope > SOUND_VOICE_THRESHOLD;
}

SoundData SoundSensorManager::getData() const {
  return data;
}
