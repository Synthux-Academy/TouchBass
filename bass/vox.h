#pragma once

#include "daisysp.h"
#include "env.h"

namespace synthux {

class Vox {
public:

  struct Config {
    Envelope::Config env_cfg;
    float sample_rate;
    int osc1_waveform_a;
    int osc1_waveform_b;
    int osc2_waveform;
  };

  enum class Osc2Mode {
    sound,
    am
  };

  Vox();
  ~Vox() {}

  void Init(const float sample_rate, const Config cfg);

  void SetOsc1Mult(const float value) { _osc1_freq_mult = value; }

  void SetOsc2Mult(const float value) { _osc2_freq_mult = value; }

  void SetOsc2Amount(const float value) { _osc2_amount = value; }

  void SetOsc2Mode(const Osc2Mode mode) { _osc2_mode = mode; } 

  void SetOsc1Shape(const float value) {
    if (value < .5f) _osc1.SetWaveform(_cfg.osc1_waveform_a);
    else _osc1.SetWaveform(_cfg.osc1_waveform_b);
  }

void NoteOn(float freq, float amp, bool retrigger);

void NoteOff() { _env.Release(); }

void SetEnvelope(const float value) { _env.SetShape(value); }

void SetEnvelopeMode(const Envelope::Mode mode) { _env.SetMode(mode); } 

float Process();

private:
  static constexpr float kSlopeMin  = 0.01f;
  static constexpr float kSlopeMax  = 1.99f;

  daisysp::Oscillator _osc1;
  daisysp::Oscillator _osc2;
  synthux::Envelope _env;

  Config _cfg;

  float _sample_rate;
  float _base_freq;
  float _pending_freq;
  float _osc1_freq_mult;
  float _osc2_freq_mult;
  float _osc2_amount;
  Osc2Mode _osc2_mode;
  bool _is_pending;
};

};
