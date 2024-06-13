#include "vox.h"

using namespace synthux;

Vox::Vox(): 
_osc1_freq_mult { 1.f },
_osc2_freq_mult { 1.f },
_osc2_mode      { Osc2Mode::sound }
{};

void Vox::Init(const float sample_rate, const Config cfg) {
  _sample_rate = sample_rate;
  _cfg = cfg;
  _osc1.Init(sample_rate);
  _osc1.SetWaveform(cfg.osc1_waveform_a);
  _osc2.Init(sample_rate);
  _osc2.SetWaveform(cfg.osc2_waveform);
  _env.Init(sample_rate, cfg.env_cfg);
};

void Vox::NoteOn(float freq, float amp, bool retrigger) {
  if (retrigger) {
    _env.Reset();
    _pending_freq = freq;
  }
  else {
    _base_freq = freq;
    _env.Trigger();
  }
};

float Vox::Process() {
  auto out = 0.f;
  auto env = _env.Process();
  if (_env.IsRunning()) {
    auto osc1_amp = env;
    auto osc2_out = _osc2.Process() * _osc2_amount;
    auto osc2_base_freq = _base_freq;
    switch (_osc2_mode) {
      case Osc2Mode::sound: 
        out = osc2_out * env;
        break;
      case Osc2Mode::am: 
        osc1_amp *= (1.f - _osc2_amount * (1 - osc2_out)) * 1.6 + 0.9 * _osc2_amount;
        osc2_base_freq = 5.f;
        break;
    }
    _osc1.SetFreq(_osc1_freq_mult * _base_freq);
    _osc2.SetFreq(_osc2_freq_mult * osc2_base_freq);

    out += _osc1.Process() * osc1_amp;
  }
  else if (_pending_freq > 0) {
    _base_freq = _pending_freq;
    _pending_freq = 0;
    _env.Trigger();
  }
  return out * .75f;
};
