#include "trigger.h"
#include <algorithm>

using namespace synthux;

Trigger::Trigger(size_t ppqn, Every resolution):
_swing_kof          { 0 },
_swing              { 0 },
_pulses_per_bar     { ppqn * 4 },
_pulses_per_trigger { ppqn / static_cast<size_t>(resolution) },
_triggers_per_bar   { 4 * static_cast<size_t>(resolution) },
_iterator           { 0 },
_trigger_count      { 0 },
_next_trigger       { 0 },
_odd_count          { 0 },
_odd_count_max      { 0 },
_is_odd             { false },
_swing_on           { false }
{
  if (resolution == Every::_16th || resolution == Every::_32th) {
    _swing_on = true;
    _swing_kof = 5.f * static_cast<float>(ppqn) / 48.f; //48ppqn is a reference
    _odd_count_max = static_cast<size_t>(resolution) / 4;
  }
};

bool Trigger::Tick() {
    auto should_trigger = (_iterator == _next_trigger);
    if (++_iterator == _pulses_per_bar) _iterator = 0;

    if (should_trigger) {
      if (++_trigger_count == _triggers_per_bar) _trigger_count = 0;
      _next_trigger = _trigger_count * _pulses_per_trigger;

      if (_swing_on && ++_odd_count == _odd_count_max) {
        _is_odd = !_is_odd;
        if (_is_odd) _next_trigger += _swing;
        _odd_count = 0;
      }
    }
    
    return should_trigger;
};

void Trigger::Reset() {
  _iterator = 0;
  _trigger_count = 0;
  _next_trigger = 0;
  _is_odd = false;
  _odd_count = 0;
};
