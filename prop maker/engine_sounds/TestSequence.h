// TestSequence.h - the 25 s bench test, repeated forever
//
//   0 -  5 s  idle
//   5 - 15 s  throttle ramps 0 -> 100 % over 5 s, then back to 0 % over 5 s
//  15 - 20 s  reversing beeper (with a little throttle, like creeping backwards)
//  20 - 25 s  hydraulic pump working (bucket lever at 100 %)
//
// Later, replace this with real inputs: throttle from the RC receiver / ESC signal,
// reversing when the drive goes backwards, hydraulic from the bucket channel.

#pragma once
#include "EngineSound.h"

const uint32_t TEST_CYCLE_MS = 25000;

// Sets the engine inputs for time tMs and returns the name of the current phase.
inline const char *runTestSequence(EngineSound &s, uint32_t tMs) {
  uint32_t t = tMs % TEST_CYCLE_MS;

  if (t < 5000) {                       // 1) idle
    s.setThrottle(0); s.setReversing(false); s.setHydraulic(0);
    return "1/4 idle";
  }
  if (t < 15000) {                      // 2) rev up slowly, then down
    uint32_t u = t - 5000;              // 0..9999 ms
    int pct = (u < 5000) ? (int)(u / 50) : (int)((10000 - u) / 50);
    s.setThrottle(pct); s.setReversing(false); s.setHydraulic(0);
    return (u < 5000) ? "2/4 revving up" : "2/4 revving down";
  }
  if (t < 20000) {                      // 3) reversing beeps
    s.setThrottle(20); s.setReversing(true); s.setHydraulic(0);
    return "3/4 reversing";
  }
  s.setThrottle(0); s.setReversing(false); s.setHydraulic(100);  // 4) hydraulics
  return "4/4 hydraulic pump";
}
