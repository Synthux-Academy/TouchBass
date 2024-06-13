#pragma once

#include <math.h>

namespace synthux {

enum class Every {
  _4th  = 1,
  _8th  = 2,
  _16th = 4,
  _32th = 8
};

class Trigger {
public:
  Trigger(size_t ppqn, Every resolution = Every::_16th);

  void SetSwing(const float frac_swing) {
      // For 48 ppqn. Conversion to actual ppqn is
      // considered in _swing_kof ////////////////
      // |  0  |  1  |  2  |  3  |  4  |  5  |
      // | 50% | 54% | 58% | 62% | 66% | 70% |
      if (_swing_on) _swing = static_cast<size_t>(std::round(frac_swing * _swing_kof));
  }

  bool Tick();

  void Reset();

private:
    float _swing_kof;
    size_t _swing;
    size_t _pulses_per_bar;
    size_t _pulses_per_trigger;
    size_t _triggers_per_bar;
    size_t _iterator;
    size_t _trigger_count;
    size_t _next_trigger;
    size_t _odd_count;
    size_t _odd_count_max;
    bool _is_odd;
    bool _swing_on;
};

};
