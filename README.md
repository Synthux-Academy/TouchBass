# THIS IS SIMPLE BASS

## QUICK INSTALL
Download the [Binary file](https://github.com/Synthux-Academy/TouchBass/releases/latest/download/TouchBass.bin) and flash using the [Daisy Seed web programmer](https://electro-smith.github.io/Programmer/)

## CONTROLS
<img src="touch.jpeg" width="300"/>

**Switches**
- S07-S08 - arpeggiator off/on/latch
- S09-S10 - Osc 2 mode: \\\ → sound, || and // → AM.     

**Knobs**
- S30 - Osc 2 amount
- S31 - Osc 1 pitch +/- 1 octave
- S31 + P10 - Osc 1 waveform saw/square
- S32 - Osc 2 pitch +/- 1 octave
- S33 - pattern
- S34 - pitch randomisation
- S35 - envelope randomisation
- S35 + P10 - reverb mix
- S36 - filter cutoff
- S36 + P10 - filter resonance
- S36 + P11 - filter envelope amount
- S37 - envelope

**Pads**
- P10 + P0/P02 - arpeggiator speed +/- (to engage ext. clock set to minimum)
- P10 + P08/P09 - external clock slower / faster
- P11 + P0/P02 - scale (one of the three)
- P03...P09 - notes
- P10 + P11 - monophonic / paraphonic mode

**LED**
- On while the arpeggiator is latched
- Three flashes when an increment control hits its lowest or highest value
  
## MIDI CC
- 71 Filter Resonance
- 72 Envelope
- 74 Filter Cut Off
- 75 Osc 1 Freq
- 76 Osc 1 Shape
- 77 Osc 2 Freq
- 78 Osc 2 Amount
- 85 Pattern
- 86 Note Randomisation
- 87 Envelope randomisation
- 91 Reverb amount
- 126 Set mono
- 127 Set poly (paraphonic)

## PREREQUISITES
- [Daisy Toolchain](https://daisy.audio/tutorials/cpp-dev-env/) (ARM GCC + make)
- **Windows:** use [Git Bash](https://git-scm.com/downloads) to run the commands below — cmd and PowerShell won't work

## PROJECT SETUP
```shell
$ git clone --recurse-submodules https://github.com/Synthux-Academy/TouchBass.git
$ cd TouchBass/lib/libDaisy
$ make
$ cd ../DaisySP
$ make
$ cd ../..
$ make clean; make
```

If you already have the repo cloned without submodules, run this first:
```shell
$ git submodule update --init --recursive
```

## CONFIGURATION
Use [config.h](https://github.com/Synthux-Academy/TouchBass/blob/main/config.h) for changing scales, ranges, tweaking arpeggiator behavior.

## EXTERNAL CLOCK SYNC
Sync to MIDI clock over USB, or to an analog clock by soldering a mono jack to a free analog pin: tip to **D28 (S43)**, sleeve to any ground pad. D28 is 5V tolerant, so most clock outputs (volca, Pocket Operator, Eurorack, KeyStep) connect directly, no extra components. Change `kClockInPin` in [TouchBass.cpp](https://github.com/Synthux-Academy/TouchBass/blob/main/TouchBass.cpp) to use a different pin.

Sync engages when the tempo is turned all the way **down**: hold P10 and tap P0 until it bottoms out, eight taps from the default, and the LED flashes. P10 + P02 steps back out to the internal clock. Keep the pattern knob (S33) low at first, or it can look like nothing is happening.

Both clock sources feed the same input, so use one at a time. Match the incoming pulse rate with **P10 + P08 / P09**, which doubles as a divider once locked. It starts at 24 PPQN (MIDI clock); volca and Pocket Operator send 2, Eurorack clocks are often 4 or 8. If the synth runs far too slow against your clock, this is the setting to change.

| Setting | 192 | 96 | 48 | **24** | 12 | 8 | 6 | 4 | 2 |
|---|---|---|---|---|---|---|---|---|---|
| Speed | /8 | /4 | /2 | **x1** | x2 | x3 | x4 | x6 | x12 |

While P10 is held, P08 and P09 act as this control instead of playing their notes.

## UPLOAD
```shell
$ make program-dfu
```

> [!NOTE]
> When tweaking code, run `make clean && make` for a full rebuild, or just `make` for an incremental rebuild (only recompiles changed files). The compiled binary is placed in the `build/` folder as `TouchBass.bin`.