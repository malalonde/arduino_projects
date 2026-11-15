// engine_sounds.ino - RC wheel loader engine sound test for the
// Adafruit RP2040 Prop-Maker Feather (built-in I2S class-D amp, speaker on the screw terminals)
//
// Board package: "Raspberry Pi Pico/RP2040" by Earle F. Philhower (arduino-pico)
// Board:         Adafruit Feather RP2040 Prop-Maker
//
// Core 0 runs the test sequence and the engine model (update()).
// Core 1 does nothing but mix samples and push them to the I2S amp at 22050 Hz.
//
// Sound recipe and samples: TheDIYGuy999, github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32

// ---- choose the engine sound (comment one, uncomment the other) ----
#define SOUND_CAT980
//#define SOUND_VOLVO_L120H

const int MASTER_VOLUME = 20;   // percent. 100 = full scale; the amp is loud, start low

#include <I2S.h>
#include "EngineSound.h"
#include "Profiles.h"
#include "TestSequence.h"

// Prop-Maker pins (the board variant normally defines these already)
#ifndef PIN_I2S_DATA
#define PIN_I2S_DATA 16
#endif
#ifndef PIN_I2S_BIT_CLOCK
#define PIN_I2S_BIT_CLOCK 17     // word select must be the next pin (18), which it is on this board
#endif
#ifndef PIN_EXTERNAL_POWER
#define PIN_EXTERNAL_POWER 23    // powers the amp and the NeoPixel output
#endif

I2S i2s(OUTPUT);
EngineSound engine;
volatile bool engineReady = false;

// ---------------- core 0: control ----------------
void setup() {
  Serial.begin(115200);

  pinMode(PIN_EXTERNAL_POWER, OUTPUT);
  digitalWrite(PIN_EXTERNAL_POWER, HIGH);   // the amp stays silent without this

  engine.begin(ENGINE_PROFILE, EXTRAS_PROFILE);
  engine.setMasterVolume(MASTER_VOLUME);
  engineReady = true;
}

void loop() {
  static uint32_t lastUpdate = 0, lastPrint = 0;
  static const char *lastPhase = "";
  uint32_t now = millis();

  if (now - lastUpdate >= 2) {              // engine model at ~500 Hz
    lastUpdate = now;
    const char *phase = runTestSequence(engine, now);
    engine.update(now);

    if (phase != lastPhase) {
      Serial.print("\n--- ");
      Serial.println(phase);
      lastPhase = phase;
    }
  }

  if (now - lastPrint >= 250) {             // RPM readout for checking the ramp
    lastPrint = now;
    Serial.print("rpm ");
    Serial.println(engine.rpmValue());
  }
}

// ---------------- core 1: audio ----------------
void setup1() {
  i2s.setBCLK(PIN_I2S_BIT_CLOCK);
  i2s.setDATA(PIN_I2S_DATA);
  i2s.setBitsPerSample(16);
  if (!i2s.begin(ENGINE_SAMPLE_RATE)) {
    while (true) {
      Serial.println("I2S failed to start");
      delay(1000);
    }
  }
}

void loop1() {
  if (!engineReady) return;
  int16_t s = engine.render();
  while (i2s.write16(s, s) == 0) { }        // waits for room in the I2S buffer -> paces at 22050 Hz
}
