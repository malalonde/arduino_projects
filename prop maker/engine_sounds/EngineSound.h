// EngineSound.h - minimal RC engine sound mixer
//
// A stripped-down version of the recipe used by TheDIYGuy999's Rc_Engine_Sound_ESP32:
//   * an idle loop and a rev loop, both played FASTER as RPM rises (that is what makes the pitch climb)
//   * the idle loop cross-fades into the rev loop between revSwitchPoint and idleEndPoint
//   * engine volume rises with throttle (and with hydraulic load)
//   * RPM follows the throttle with limited acceleration/deceleration (engine inertia)
//   * reversing beeper and hydraulic pump + oil-flow hiss are mixed on top at fixed speed
//
// No hardware code in here: the sketch calls update() from the main loop and
// render() once per audio sample (22050 Hz) and sends the result to the I2S amp.
// Thread-safety: update() runs on core 0, render() on core 1. They only share
// aligned 32-bit values, which the RP2040 reads and writes atomically.

#pragma once
#include <stdint.h>

#define ENGINE_SAMPLE_RATE 22050

struct SoundClip {
  const int8_t *data;
  uint32_t count;
};

// Tuning values. Names and meaning follow TheDIYGuy999's vehicle files
// (RPM scale is 0..500, where 0 = idle and 500 = full RPM).
struct EngineProfile {
  SoundClip idle;
  SoundClip rev;
  int idleVolumePct;           // idle loop volume
  int revVolumePct;            // rev loop volume
  int engineIdleVolumePct;     // engine volume at zero throttle
  int fullThrottleVolumePct;   // engine volume at full throttle
  int revSwitchPoint;          // RPM where the rev loop starts fading in
  int idleEndPoint;            // RPM where the idle loop is fully faded out
  int idleVolumeProportionPct; // share of idle loop below revSwitchPoint (rest is rev loop)
  int maxRpmPct;               // playback speed at full RPM (200 = twice as fast as recorded)
  float rpmRisePerSec;         // engine inertia: RPM units per second when speeding up
  float rpmFallPerSec;         // ... and when slowing down
};

struct ExtrasProfile {
  SoundClip reverseBeep;
  SoundClip hydPump;
  SoundClip hydFlow;
  int reversingVolumePct;
  int hydraulicPumpVolumePct;
  int hydraulicFlowVolumePct;
  int hydraulicLoadPct;        // how much the engine "works harder" (volume) at full hydraulic lever
  float hydRampPerSec;         // pump fade in/out speed, in lever-% per second
};

class EngineSound {
public:
  void begin(const EngineProfile &e, const ExtrasProfile &x) {
    eng = e;
    ext = x;
    rpm = 0;
    hyd = 0;
    lastMs = 0;
    started = false;
    update(0);
  }

  // ---- control inputs (call from the main loop) ----
  void setThrottle(int pct)      { throttleIn = clamp(pct, 0, 100); }   // 0..100 %
  void setHydraulic(int pct)     { hydIn = clamp(pct, 0, 100); }        // 0..100 % (bucket lever)
  void setReversing(bool on)     { reversingIn = on; }
  void setMasterVolume(int pct)  { masterQ8 = clamp(pct, 0, 400) * 256 / 100; }

  int rpmValue() const { return (int)rpm; } // 0..500, handy for Serial debugging

  // ---- call often (every 1-5 ms) ----
  void update(uint32_t nowMs) {
    float dt = started ? (nowMs - lastMs) / 1000.0f : 0.0f;
    started = true;
    lastMs = nowMs;
    if (dt > 0.1f) dt = 0.1f;

    // Engine inertia: RPM chases the throttle at a limited rate
    float target = throttleIn * 5.0f;
    if (rpm < target) { rpm += eng.rpmRisePerSec * dt; if (rpm > target) rpm = target; }
    else              { rpm -= eng.rpmFallPerSec * dt; if (rpm < target) rpm = target; }

    // Hydraulic lever is smoothed so the pump fades in/out instead of clicking
    float hydTarget = (float)hydIn;
    float step = ext.hydRampPerSec * dt;
    if (hyd < hydTarget) { hyd += step; if (hyd > hydTarget) hyd = hydTarget; }
    else                 { hyd -= step; if (hyd < hydTarget) hyd = hydTarget; }

    // Playback speed (Q16 fixed point): 1.0 at idle up to maxRpmPct/100 at full RPM
    float speed = 1.0f + (rpm / 500.0f) * (eng.maxRpmPct - 100) / 100.0f;
    engineStepQ16 = (int32_t)(speed * 65536.0f);

    // Engine volume follows throttle, or the hydraulic load if that is larger
    float load = throttleIn * 5.0f;
    float hydLoad = hyd * 5.0f * ext.hydraulicLoadPct / 100.0f;
    if (hydLoad > load) load = hydLoad;
    float engVol = eng.engineIdleVolumePct + (eng.fullThrottleVolumePct - eng.engineIdleVolumePct) * load / 500.0f;

    // Idle -> rev cross-fade (same rule as the original project)
    float idleShare;
    if (rpm <= eng.revSwitchPoint)     idleShare = eng.idleVolumeProportionPct;
    else if (rpm >= eng.idleEndPoint)  idleShare = 0;
    else idleShare = eng.idleVolumeProportionPct * (eng.idleEndPoint - rpm) / (float)(eng.idleEndPoint - eng.revSwitchPoint);

    idleGainQ8 = (int32_t)(256.0f * engVol / 100 * eng.idleVolumePct / 100 * idleShare / 100);
    revGainQ8  = (int32_t)(256.0f * engVol / 100 * eng.revVolumePct / 100 * (100 - idleShare) / 100);
    pumpGainQ8 = (int32_t)(256.0f * hyd / 100 * ext.hydraulicPumpVolumePct / 100);
    flowGainQ8 = (int32_t)(256.0f * hyd / 100 * ext.hydraulicFlowVolumePct / 100);
    beepGainQ8 = (int32_t)(256.0f * ext.reversingVolumePct / 100);
    beepOn = reversingIn ? 1 : 0;
  }

  // ---- call once per output sample (22050 times per second) ----
  int16_t render() {
    int32_t step = engineStepQ16;
    int32_t mix = 0;

    // Engine: both loops run at the RPM-dependent speed
    mix += readLoop(eng.idle, idlePos, step) * idleGainQ8;
    mix += readLoop(eng.rev, revPos, step) * revGainQ8;

    // Hydraulics (fixed speed). Positions keep running so restarting does not click.
    if (pumpGainQ8 > 0) mix += readFixed(ext.hydPump, pumpPos) * pumpGainQ8;
    if (flowGainQ8 > 0) mix += readFixed(ext.hydFlow, flowPos) * flowGainQ8;

    // Reversing beeper: once started, a 1 s beep cycle always finishes
    if (beepOn || beepPos != 0) {
      mix += (int32_t)ext.reverseBeep.data[beepPos] * 256 * beepGainQ8;
      if (++beepPos >= ext.reverseBeep.count) beepPos = 0;
    }

    // mix is int8 * 256 (Q8 interpolation) * gain(Q8) -> scale to 16-bit and apply master volume
    int32_t out = (mix >> 8) * masterQ8 >> 8;
    if (out > 32767) out = 32767;
    if (out < -32768) out = -32768;
    return (int16_t)out;
  }

private:
  // Linear-interpolated read from a looping clip at variable speed; returns sample * 256.
  // Position is 16.16 fixed point, so these clips must be shorter than 65536 samples (~3 s).
  static int32_t readLoop(const SoundClip &c, uint32_t &posQ16, int32_t stepQ16) {
    uint32_t i = posQ16 >> 16;
    uint32_t frac = (posQ16 & 0xFFFF) >> 8; // 0..255
    uint32_t i2 = (i + 1 < c.count) ? i + 1 : 0;
    int32_t s0 = c.data[i], s1 = c.data[i2];
    int32_t s = s0 * 256 + (s1 - s0) * (int32_t)frac;
    posQ16 += stepQ16;
    uint32_t end = c.count << 16;
    if (posQ16 >= end) posQ16 -= end;
    return s;
  }
  // Plain looping read at the recorded speed (any length); returns sample * 256
  static int32_t readFixed(const SoundClip &c, uint32_t &pos) {
    int32_t s = (int32_t)c.data[pos] * 256;
    if (++pos >= c.count) pos = 0;
    return s;
  }
  static int clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

  EngineProfile eng;
  ExtrasProfile ext;

  // inputs
  volatile int throttleIn = 0, hydIn = 0;
  volatile bool reversingIn = false;

  // control state (core 0)
  float rpm = 0, hyd = 0;
  uint32_t lastMs = 0;
  bool started = false;

  // shared with the audio core
  volatile int32_t engineStepQ16 = 65536;
  volatile int32_t idleGainQ8 = 0, revGainQ8 = 0, pumpGainQ8 = 0, flowGainQ8 = 0, beepGainQ8 = 0;
  volatile int32_t beepOn = 0;
  volatile int32_t masterQ8 = 128;

  // playback positions (core 1 only)
  uint32_t idlePos = 0, revPos = 0, pumpPos = 0, flowPos = 0, beepPos = 0;
};
