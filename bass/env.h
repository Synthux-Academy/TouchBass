#pragma once

#include <algorithm>
#include "../nocopy.h"

namespace synthux {

class Envelope {
public:
  struct Config {
    float time_range;
    float attack_min;
    float decay_a_min;
    float decay_b_min;
  };

  enum class Mode {
    AR,
    ASR
  };

  Envelope();
  ~Envelope() {};

  void Init(const float sample_rate, const Config cfg);
  
  bool IsRunning() { return _stage != Stage::idle; }

  void SetMode(const Mode mode) { _mode = mode; }

  void SetShape(const float value);
  
  void Trigger();

  void Release();

  float Process();
  
  void Reset();

private:
  NOCOPY(Envelope)

  enum class Stage {
    idle,
    attack,
    sustain,
    decay,
    reset
  };

  void _set_curve(const float value) {
    auto cu = value - .5f;
    _curve_kof = 128.f * cu * cu;
  }

  // Derived from Stages by Emilie Gillet ///////
  float _amp_attack(const float ph) {
    return ph / (1.f + _curve_kof * (1.f - ph));
  }
  float _amp_decay(const float ph) {
    return (1.f - ph) / (1.f + _curve_kof * ph);
  }
  size_t _ph_attack(float amp) {
    return static_cast<size_t>(roundf(_t_attack * amp * (1 + _curve_kof) / (1 + amp * _curve_kof)));
  }
  size_t _ph_decay(float amp) {
    return static_cast<size_t>(roundf(_t_decay * (1 - amp) / (amp * _curve_kof + 1)));
  }
  ////////////////////////////////////////////////
  
  float _out;
  float _curve_kof;
  float _t_min_attack;
  float _t_min_decay_a;
  float _t_min_decay_b;
  float _t_range_2x;
  float _t_reset_out;
  float _t_reset;
  float _t_reset_kof;
  float _t_attack_kof;
  float _t_decay_kof;
  size_t _t_attack;
  size_t _t_decay;
  size_t _phase;
  Mode _mode;
  Stage _stage;
};

};
