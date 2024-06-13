#pragma once
#include "daisysp.h"
#include "env.h"
#include "../nocopy.h"

namespace synthux {

class Filter {
public:
  struct Config {
    float freq_min;
    float freq_max;
    Envelope::Config env_cfg;
  };

  Filter();
  ~Filter() {}

  void Init(const float sample_rate, const Config cfg);

  void Trigger(bool retrigger = false);

  void Release() { _env.Release(); }

  void SetEnvelope(const float value) { _env.SetShape(value); }

  void SetEnvelopeMode(const Envelope::Mode mode) { _env.SetMode(mode); } 

  void SetEnvelopeAmount(const float value) { _env_amount = value; }

  void SetFreq(const float value) {
    _freq = daisysp::fmap(value, _cfg.freq_min, _cfg.freq_max);
    _freq_env_room = _cfg.freq_max - _freq;
  }

  void SetReso(const float value) { _flt.SetRes(daisysp::fmap(value, 0.f, .9f)); }

  float Process(const float in);

private:
  NOCOPY(Filter)

  daisysp::Svf _flt;
  Envelope _env;
  Config _cfg;
  float _freq;
  float _freq_env_room;
  float _env_amount;
  bool _pending_retrigger;
};

};
