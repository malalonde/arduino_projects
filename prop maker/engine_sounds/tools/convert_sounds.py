# Convert TheDIYGuy999 sound headers into uniquely-named int8 arrays.
# Usage: python convert_sounds.py <Rc_Engine_Sound_ESP32/src/vehicles/sounds> <engine_sounds/src/sounds>
# Add entries to "sel" to bring in more sounds (all must be 22050 Hz).
import re, sys, os
src, outdir = sys.argv[1], sys.argv[2]
sel = {  # new_name : (source file, description)
 'cat980_idle':   ('CAT730Idle.h',  'CAT 730/980 idle loop (used by the CAT980 wheel loader profile)'),
 'cat980_rev':    ('CAT730Rev.h',   'CAT 730/980 rev loop (used by the CAT980 wheel loader profile)'),
 'volvo_idle':    ('L120H_BaumaIdleOutside.h', 'Volvo L120H wheel loader idle loop'),
 'volvo_rev':     ('VolvoL120HIdle.h', 'Volvo L120H wheel loader engine loop used as rev sound'),
 'reverse_beep':  ('TruckReversingBeep.h', '1000 Hz reversing beeper, 1 s cycle'),
 'hyd_pump':      ('Caterpillar323Hydraulic2.h', 'Caterpillar 323 hydraulic pump loop (slowed x2)'),
 'hyd_flow':      ('hydraulicHiss.h', 'Hydraulic oil flow hiss'),
}
for name,(f,desc) in sel.items():
    t = open(os.path.join(src,f), encoding='utf-8', errors='replace').read()
    rate = int(re.search(r'[Ss]ampleRate\s*=\s*(\d+)', t).group(1))
    body = t.split('[]',1)[1].split('{',1)[1].split('}',1)[0]
    body = re.sub(r'//[^\n]*', '', body)
    v = [int(x) for x in re.findall(r'-?\d+', body)]
    assert rate == 22050, (f, rate)
    if name == 'reverse_beep':
        # Original file: 0.5 s of 1 kHz tone, then 0.5 s held at about -124 (a DC level that the
        # ESP32's DAC tolerated). On an I2S amp that DC step thumps the speaker, so replace it with true silence.
        i = next(k for k in range(1000, len(v)-200) if all(x < -100 for x in v[k:k+200]))
        j = max(k for k in range(1, i) if v[k-1] > 0 and v[k] <= 0)
        v = v[:j] + [0]*(len(v)-j)
        desc += ' (silent half zeroed from sample %d, DC removed)' % j
    lines = [', '.join(str(x) for x in v[i:i+24]) for i in range(0,len(v),24)]
    with open(os.path.join(outdir, name+'.h'),'w') as o:
        o.write(f'// {desc}\n// Source: github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32  src/vehicles/sounds/{f}\n')
        o.write(f'// 8-bit signed, 22050 Hz, {len(v)} samples ({len(v)/22050:.2f} s). Auto-converted, names made unique.\n#pragma once\n#include <stdint.h>\n\n')
        o.write(f'const uint32_t {name}_count = {len(v)};\nconst int8_t {name}[] = {{\n' + ',\n'.join(lines) + '\n};\n')
    print(name, len(v))
