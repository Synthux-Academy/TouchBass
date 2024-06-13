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
  void SpeedUp() {
    _tempo = std::min(_tempo + .05f, 1.f);
    SetTempo(_tempo);
  };
  void SlowDown() {
      _tempo = std::max(_tempo - .05f, 0.05f);
      SetTempo(_tempo);
  };
  void ProcessClockIn(const bool state) { _clock.Process(state); }

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
  static constexpr uint8_t kPPQNExtern = 24;
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
  
  float   _tempo;
  float   _env;
  float   _human_env_kof;
  uint8_t _random_note_chance;
  uint8_t _human_env_chance;
  uint8_t _scale_index;
  
  bool _is_arp_on;
  bool _is_latched;
  std::array<bool, 128> _hold;
};

};
