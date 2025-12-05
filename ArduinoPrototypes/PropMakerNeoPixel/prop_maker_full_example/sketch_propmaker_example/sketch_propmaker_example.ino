#include <I2S.h>
#include "jingle.h"


const uint32_t hithereSampleRate = 22050;
const uint8_t hithereAudioData[] PROGMEM = {128,128,128,128,128};


I2S i2s(OUTPUT);  // 👈 declare the I2S object

const uint32_t sampleRate = 22050;

float volume = 0.02;   // 👈 Volume scale (0.0 = silent, 1.0 = full volume)

// Simple note-to-frequency map
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5 740
#define NOTE_G5  784
#define NOTE_A5  880

// Jingle Bells melody (fragment)
int melody[] = {
  NOTE_E5, NOTE_E5, NOTE_E5,    // Jingle bells
  NOTE_E5, NOTE_E5, NOTE_E5,    // Jingle bells
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5, NOTE_E5,  // Jingle all the way
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_G5
};

int noteDurations[] = {
  4,4,2,
  4,4,2,
  4,4,4,4,2,
  4,4,4,4,4,4,4,4,4,4,4,4,4,2
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("RP2040 PropMaker I2S Audio Test");

  pinMode(PIN_EXTERNAL_POWER, OUTPUT);
  digitalWrite(PIN_EXTERNAL_POWER, HIGH);

  i2s.setBCLK(PIN_I2S_BIT_CLOCK);
  i2s.setDATA(PIN_I2S_DATA);

  // Initialize I2S for 16-bit mono audio
  if (!i2s.begin(hithereSampleRate)) {
    Serial.println("Failed to initialize I2S!");
    while (1);
  }

  Serial.println("Playing sound...");
  playSound(hithereAudioData, sizeof(hithereAudioData));
  Serial.println("Done.");

  
  playJingleBells();
}

void loop() {
  delay(1000);
  
  //playSound(hithereAudioData, sizeof(hithereAudioData));
  //   for (int i = 0; i < 100; i++) {
  //   int16_t sample = (int16_t)(sin(2 * 3.14159 * i / 100.0) * 32767);
  //   i2s.write(sample);
  //   i2s.write(sample);
  // }

  
  //playJingleBells();
}

// Play 8-bit unsigned PCM audio data
void playSound(const uint8_t *data, uint32_t len) {
  for (uint32_t i = 0; i < len; i++) {
    int16_t sample = (((int16_t)data[i] - 128) << 8)*volume;  // convert 8-bit to signed 16-bit
    i2s.write(sample);
    i2s.write(sample);
  }
}


void playTone(float freq, int durationMs) {
  int samples = (sampleRate * durationMs) / 1000;
  for (int i = 0; i < samples; i++) {
    float t = (float)i / sampleRate;
    int16_t sample = (int16_t)(sin(2 * PI * freq * t) * 32767)*volume;
    i2s.write(sample);
    i2s.write(sample);
  }
}

void playJingleBells() {
  int numNotes = sizeof(melody) / sizeof(melody[0]);
  for (int i = 0; i < numNotes; i++) {
    int duration = 600 / noteDurations[i];
    playTone(melody[i], duration);
    delay(duration * 0.3); // short pause between notes
  }
}

void playWavFromFlash() {
  // Find start of PCM data (after 'data' chunk header)
  const uint8_t *ptr = jingle_wav;
  uint32_t len = jingle_wav_len;

  // Skip the 44-byte WAV header
  if (len > 44) {
    ptr += 44;
    len -= 44;
  }

  // Stream 16-bit little-endian PCM samples
  for (uint32_t i = 0; i < len; i += 2) {
    int16_t sample = (ptr[i] | (ptr[i + 1] << 8))*volume;
    i2s.write(sample);
    i2s.write(sample);
  }
}
