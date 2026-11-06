// SYNTHUX ACADEMY /////////////////////////////////////////
// SIMPLE BASS /////////////////////////////////////////////
#pragma once
#include <array>
#include <random>

#include "daisysp.h"

#include "../nocopy.h"
#include "../config.h"

#include "synclock.h"
#include "trigger.h"
#include "cpattern.h"
#include "arp.h"
#include "driver.h"
#include "scale.h"
#include "vox.h"
#include "flt.h"
#include "xfade.h"

namespace synthux {

class Bass {
public:
  struct VoxParams {
    float osc1_shape;
    float osc1_pitch;
    float osc2_pitch;
    float osc2_amnt;
    float env;
    uint8_t osc2_mode_index;
  };

  struct FilterParams {
    float freq;
    float reso;
    float env_amount;
  };

  Bass();
  ~Bass() {}
  
  void Init(const float sample_rate, const float buffer_size);
  
  void SetTempo(const float tempo) { _clock.SetTempo(tempo); }
  // Increment controls return true when the value is at an end stop.
  bool SpeedUp() {
    _tempo = std::min(_tempo + .05f, 1.f);
    SetTempo(_tempo);
    return _tempo >= 1.f;
  };
  bool SlowDown() {
      _tempo = std::max(_tempo - .05f, 0.05f);
      SetTempo(_tempo);
      return _tempo <= 0.05f;
  };
  void ProcessClockIn(const bool state) { _clock.Process(state); }

  bool ClockSlower() {
    if (_clock_ppqn_idx > 0) _clock_ppqn_idx --;
    _clock.SetPPQNIn(kClockPPQN[_clock_ppqn_idx]);
    return _clock_ppqn_idx == 0;
  };
  bool ClockFaster() {
    if (_clock_ppqn_idx < kClockPPQN.size() - 1) _clock_ppqn_idx ++;
    _clock.SetPPQNIn(kClockPPQN[_clock_ppqn_idx]);
    return _clock_ppqn_idx == kClockPPQN.size() - 1;
  };

  void SetArpOn(const bool value);
  bool IsLatched() { return _is_latched; }
  void SetLatch(const bool latch);

  void SetMono() { _driver.SetMono(); };
  void SetPoly() { _driver.SetPoly(); };
  bool IsMono() { return _driver.IsMono(); };

  void NoteOn(const uint8_t note);
  void NoteOff(const uint8_t note);
  void AllNotesOff();

  void Reset();

  void SetPattern(const float value) { _pattern.SetOnsets(value); }

  void SetRandomNoteScaleIndex(const uint8_t index) {
    _scale.SetRandomScaleIndex(index);
  }
  void SetRandomNoteChance(const float value) { 
    _random_note_chance = std::clamp<int>(value * 100, 0, 100); 
    _arp.SetRandChance(_random_note_chance);
  }
  void SetHumanEnvelopeChance(const float value) {
    _human_env_chance = std::clamp<int>(value * 100, 0, 100);
    _human_env_kof = _human_env_chance * 0.0001f;
  };

  void SetVoxParams(const VoxParams& p);
  void SetFilterParams(const FilterParams& p);

  void SetReverbMix(const float value) { _xfade.SetStage(value);  }

  void Process(float **out, size_t size);

private:
  NOCOPY(Bass)

  void _on_clock_tick();
  void _on_arp_note_on(uint8_t num, uint8_t vel);
  void _on_arp_note_off(uint8_t num);

  void _on_driver_note_on(uint8_t vox_idx, uint8_t num, bool retrigger);
  void _on_driver_note_off(uint8_t vox_idx);

  float _humanized_envelope(float env, uint8_t length);
  
  static constexpr uint8_t kPPQN = 48;
  // External clock rates in pulses per quarter note, slowest to fastest.
  // Each must divide or be a multiple of kPPQN.
  static constexpr std::array<uint16_t, 9> kClockPPQN = { 192, 96, 48, 24, 12, 8, 6, 4, 2 };
  static constexpr uint8_t kClockPPQNDefault = 3; // 24, i.e. MIDI clock
  static constexpr uint8_t kNotesCount = 7;
  static constexpr uint8_t kVoxCount = 4;

  std::array<Vox, kVoxCount>  _voices;
  Driver<kVoxCount>           _driver;
  Scale                       _scale;
  SynClock                    _clock;
  Trigger                     _trigger;
  CPattern                    _pattern;
  Arp<kNotesCount, 4>         _arp;
  Filter                      _filter;
  daisysp::ReverbSc           _reverb;
  XFade                       _xfade;

  std::default_random_engine _rand_engine;
  std::uniform_int_distribution<uint8_t> _dice;

  std::array<float, 2> _reverb_in;
  std::array<float, 2> _reverb_out;
  std::array<float, 2> _bus;
  std::array<bool, 128> _hold;

  float   _tempo;
  uint8_t _clock_ppqn_idx;
  float   _env;
  float   _human_env_kof;
  uint8_t _random_note_chance;
  uint8_t _human_env_chance;
  uint8_t _scale_index;
  uint8_t _note_on_count;
  bool _is_arp_on;
  bool _is_latched;
};

};
