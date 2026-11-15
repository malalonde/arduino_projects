# engine_sounds: RC wheel loader sound test (Adafruit RP2040 Prop-Maker Feather)

Plays a 25-second test loop through the Prop-Maker's built-in I2S amplifier:

| Time | What you should hear |
|---|---|
| 0 – 5 s | Idle |
| 5 – 10 s | Engine revving up slowly (pitch and volume climb, idle loop fades into the rev loop) |
| 10 – 15 s | Revving back down to idle |
| 15 – 20 s | Reversing beeper over a lightly revved engine |
| 20 – 25 s | Hydraulic pump and oil hiss fade in, engine works harder |

Then it repeats. The Serial Monitor (115200 baud) prints each phase and the RPM (0 = idle, 500 = max) every 250 ms, so you can check that the ramp is smooth.

`preview_cat980.wav` and `preview_volvo_l120h.wav` were rendered on a PC from this same code, so you can hear the result before flashing.

## Setup in Arduino IDE

1. **File → Preferences → Additional boards manager URLs**: add
   `https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json`
2. **Tools → Board → Boards Manager**: install **Raspberry Pi Pico/RP2040** by Earle F. Philhower, III.
3. **Tools → Board**: choose **Adafruit Feather RP2040 Prop-Maker**.
4. Connect a 4–8 Ω speaker to the Prop-Maker's speaker terminals (the amp is rated for about 3 W).
5. Open `engine_sounds.ino` and upload. If the upload doesn't start, hold BOOT, tap RESET, release BOOT, then upload again.

No extra libraries are needed: `I2S` ships with the board package.

## Choosing the engine and volume

At the top of `engine_sounds.ino`:

```cpp
#define SOUND_CAT980          // CAT 980 wheel loader (CAT 730 recordings)
//#define SOUND_VOLVO_L120H   // Volvo L120H wheel loader
const int MASTER_VOLUME = 40; // percent; about 90 is the most before loud parts start clipping
```

Mixing levels, rev cross-fade points, engine inertia and the hydraulic fade time are in `Profiles.h`.

## How it works (the recipe from TheDIYGuy999)

The method follows [Rc_Engine_Sound_ESP32](https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32), reduced to its core:

- **Short engine loops played faster as RPM rises.** A 0.15 s idle recording repeats endlessly; playing it up to 2× faster (`maxRpmPct = 200`) raises both the pitch and the firing rate, which is what sounds like revving.
- **Idle-to-rev cross-fade.** Above `revSwitchPoint` the idle loop fades out and a separate rev loop fades in, fully by `idleEndPoint`.
- **Engine inertia.** RPM follows the throttle with limited rise and fall speeds, so a sudden stick movement still revs smoothly.
- **Volume follows load.** Engine volume rises with throttle, or with the bucket lever when the hydraulics are working.
- **Extras mixed on top at their recorded speed:** reversing beeper and hydraulic pump with oil-flow hiss.

Code layout:

- `engine_sounds.ino`: the hardware side. Core 0 runs the test sequence and engine model; core 1 only mixes samples and feeds the I2S amp at 22,050 Hz.
- `EngineSound.h`: the mixer and engine model, with no hardware code.
- `Profiles.h`: sound selection and tuning.
- `TestSequence.h`: the 25 s test. This is the part to replace with real RC inputs.
- `src/sounds/*.h`: 8-bit, 22,050 Hz sound loops converted from the original project.
- `tools/convert_sounds.py`: the converter that produced them (renames the arrays so several sounds can coexist).

## Next step: real control

Replace `runTestSequence()` with something like:

```cpp
engine.setThrottle(abs(driveStickPercent));          // from the receiver, -100..100
engine.setReversing(driveStickPercent < -5);
engine.setHydraulic(max(abs(liftPercent), abs(tiltPercent)));
engine.update(millis());
```

Read the receiver channels with `pulseIn()` or, better, with interrupts or the RP2040's PIO.

## Credits

Sound recipe and all sound samples come from TheDIYGuy999's
[Rc_Engine_Sound_ESP32](https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32).
The repository doesn't include a license file, so treat the samples as his work and keep this project for personal use unless he agrees otherwise.
The reversing-beep file was changed in one way: its silent half was stored as a DC offset (about −124), which thumps the speaker on an I2S amp, so that half was set to true zero.
