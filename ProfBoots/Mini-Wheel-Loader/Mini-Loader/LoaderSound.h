// LoaderSound.h - engine sounds for the Mini Wheel Loader (ESP32 + MAX98357A I2S amplifier)
//
// The sketch only calls two things:
//   loaderSoundBegin();                       once, at the end of setup()
//   loaderSound(driveStick, hydraulicsBusy);  whenever new controller data arrives
//   loaderSoundButton(pressed);               on/off toggle (Start / Options / + button)
//
// Everything else runs in its own small task: it mixes the engine, reversing beeper and
// hydraulic pump (EngineSound.h) and feeds the amplifier 22050 samples per second.
//
// Sound recipe and samples: TheDIYGuy999, github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32

#pragma once
#include <Arduino.h>
#include <driver/i2s.h>

// ---- Wiring: MAX98357A pin -> 3-pin header on the ProfBoots Mini-Dozer PCB ----------
//   BCLK -> header "21",  pin nearest the ESP32 (signal)
//   VIN  -> header "21",  middle pin (5V)
//   GND  -> header "21",  pin at the board edge (GND)
//   LRC  -> header "LT3", pin nearest the ESP32 (GPIO 14; its middle pin is the same signal, NOT 5V)
//   DIN  -> header "TX",  pin nearest the ESP32 (GPIO 1)
//   GAIN and SD stay unconnected.
// What this costs: the LT3 aux light output no longer switches, and the Serial Monitor
// goes quiet once the sound starts (uploading still works). LT1/LT2 lights and both
// servos are untouched. To use other pins, change the numbers here.
#define SOUND_PIN_BCLK 21
#define SOUND_PIN_LRC  14
#define SOUND_PIN_DIN  1

// ---- Settings -----------------------------------------------------------------------
#define SOUND_VOLUME 40              // percent; about 90 is the most before loud parts clip
#define SOUND_REVERSE_SIGN 1         // 1: beeper when the drive stick is pulled back. Use -1 if it beeps going forward
#define SOUND_STICK_DEADBAND 30      // same dead zone as moveMotor() (stick value, +/-512 range)

#define SOUND_CAT980                 // engine sound: CAT 980 wheel loader ...
//#define SOUND_VOLVO_L120H          // ... or Volvo L120H (comment the line above, uncomment this one)

#include "EngineSound.h"
#include "Profiles.h"

#define SOUND_BLOCK 128              // samples mixed per pass (about 6 ms)

static EngineSound loaderEngine;
static volatile bool loaderSoundOn = true;   // toggled by loaderSoundButton()

// Audio task: sleeps inside i2s_write() until the amplifier needs more samples
static void loaderSoundTask(void *) {
  static int16_t block[SOUND_BLOCK * 2];   // stereo: the same sample left and right
  uint32_t samplesDone = 0;
  int volume = SOUND_VOLUME;
  for (;;) {
    // Fade in/out over about 60 ms when sound is switched on or off (no click)
    int target = loaderSoundOn ? SOUND_VOLUME : 0;
    if (volume < target) { volume += 4; if (volume > target) volume = target; }
    if (volume > target) { volume -= 4; if (volume < target) volume = target; }
    loaderEngine.setMasterVolume(volume);

    loaderEngine.update((uint32_t)((uint64_t)samplesDone * 1000 / ENGINE_SAMPLE_RATE));
    for (int i = 0; i < SOUND_BLOCK; i++) {
      int16_t s = loaderEngine.render();
      block[2 * i] = s;
      block[2 * i + 1] = s;
    }
    samplesDone += SOUND_BLOCK;
    size_t written = 0;
    i2s_write(I2S_NUM_0, block, sizeof(block), &written, portMAX_DELAY);
  }
}

// Call once at the end of setup()
inline void loaderSoundBegin() {
  i2s_config_t cfg = {};
  cfg.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  cfg.sample_rate = ENGINE_SAMPLE_RATE;
  cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  cfg.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  cfg.dma_buf_count = 8;
  cfg.dma_buf_len = SOUND_BLOCK;
  cfg.tx_desc_auto_clear = true;           // play silence, not noise, if the task is ever late

  i2s_pin_config_t pins = {};
  pins.mck_io_num = I2S_PIN_NO_CHANGE;
  pins.bck_io_num = SOUND_PIN_BCLK;
  pins.ws_io_num = SOUND_PIN_LRC;
  pins.data_out_num = SOUND_PIN_DIN;
  pins.data_in_num = I2S_PIN_NO_CHANGE;

  if (i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL) != ESP_OK || i2s_set_pin(I2S_NUM_0, &pins) != ESP_OK) {
    Serial.println("Sound: I2S failed to start, continuing without sound");
    return;
  }

  loaderEngine.begin(ENGINE_PROFILE, EXTRAS_PROFILE);
  loaderEngine.setMasterVolume(SOUND_VOLUME);

  // Core 1 (same as loop()), one priority step above it, so driving code can't make the sound stutter
  xTaskCreatePinnedToCore(loaderSoundTask, "loaderSound", 4096, NULL, 2, NULL, 1);
}

// driveStick: the throttle stick value (-511..512). hydraulicsBusy: true while an arm/bucket motor runs.
inline void loaderSound(int driveStick, bool hydraulicsBusy) {
  int amount = abs(driveStick) - SOUND_STICK_DEADBAND;
  loaderEngine.setThrottle(amount > 0 ? amount * 100 / (512 - SOUND_STICK_DEADBAND) : 0);
  loaderEngine.setReversing(driveStick * SOUND_REVERSE_SIGN > SOUND_STICK_DEADBAND);
  loaderEngine.setHydraulic(hydraulicsBusy ? 100 : 0);
}

// Sound on/off: call with the button state on every controller update; each new press toggles
inline void loaderSoundButton(bool pressed) {
  static bool wasPressed = false;
  if (pressed && !wasPressed) loaderSoundOn = !loaderSoundOn;
  wasPressed = pressed;
}
