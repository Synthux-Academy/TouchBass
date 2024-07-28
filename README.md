# THIS IS SIMPLE BASS

## QUICK INSTALL
Download the [Binary file](https://github.com/Synthux-Academy/TouchBass/raw/main/TouchBass.bin) and flash using the [Daisy Seed web programmer](https://electro-smith.github.io/Programmer/)

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
- P10 + P0/P02 - arpeggiator speed +/-
- P11 + P0/P02 - scale (one of the three)
- P03...P09 - notes
- P10 + P11 - monophonic / paraphonic mode
  
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

## CONGIGURATION
Use [config.h](https://github.com/Synthux-Academy/TouchBass/blob/main/config.h) for changing scales, ranges, tweaking arpeggiator behavior.

## UPLOAD
```shell
$ make program-dfu
```
