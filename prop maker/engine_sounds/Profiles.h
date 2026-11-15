// Profiles.h - sound sets and tuning
//
// Pick ONE engine by leaving its #define active in engine_sounds.ino:
//   SOUND_CAT980      CAT 980 wheel loader recipe (CAT 730 idle + rev loops)
//   SOUND_VOLVO_L120H Volvo L120H wheel loader recipe
// Values are taken from TheDIYGuy999's vehicle files CAT980.h and VolvoL120H.h,
// then adapted to this simpler mixer. Tweak freely.

#pragma once
#include "EngineSound.h"

#include "src/sounds/reverse_beep.h"
#include "src/sounds/hyd_pump.h"
#include "src/sounds/hyd_flow.h"

#if defined(SOUND_VOLVO_L120H)
#include "src/sounds/volvo_idle.h"
#include "src/sounds/volvo_rev.h"
const EngineProfile ENGINE_PROFILE = {
  { volvo_idle, volvo_idle_count },
  { volvo_rev, volvo_rev_count },
  55,     // idleVolumePct   (original file: 80, lowered to match the CAT level)
  70,     // revVolumePct    (original file: 110)
  80,     // engineIdleVolumePct
  140,    // fullThrottleVolumePct
  100,    // revSwitchPoint
  400,    // idleEndPoint
  90,     // idleVolumeProportionPct
  200,    // maxRpmPct (full RPM plays the loops 2x faster)
  250.0f, // rpmRisePerSec
  150.0f, // rpmFallPerSec
};
#else // default: CAT 980
#include "src/sounds/cat980_idle.h"
#include "src/sounds/cat980_rev.h"
const EngineProfile ENGINE_PROFILE = {
  { cat980_idle, cat980_idle_count },
  { cat980_rev, cat980_rev_count },
  70,     // idleVolumePct
  70,     // revVolumePct
  60,     // engineIdleVolumePct
  90,     // fullThrottleVolumePct
  50,     // revSwitchPoint
  300,    // idleEndPoint
  100,    // idleVolumeProportionPct
  200,    // maxRpmPct
  250.0f, // rpmRisePerSec
  150.0f, // rpmFallPerSec
};
#endif

const ExtrasProfile EXTRAS_PROFILE = {
  { reverse_beep, reverse_beep_count },
  { hyd_pump, hyd_pump_count },
  { hyd_flow, hyd_flow_count },
  35,     // reversingVolumePct (original: 70, but that square wave drowned the engine on I2S)
  60,     // hydraulicPumpVolumePct (original: 120)
  20,     // hydraulicFlowVolumePct (oil hiss)
  60,     // hydraulicLoadPct: engine gets louder as if 60 % throttle at full lever
  300.0f, // hydRampPerSec: pump fades in/out over about 1/3 s
};
