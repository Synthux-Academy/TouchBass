#include "mvalue.h"
#include <math.h>

using namespace synthux;

MValue::MValue(): 
_init_value   { 0.f },
_value        { 0.f },
_is_active    { false },
_is_tracking  { false }
{};

float MValue::Process(const float value, const bool active) {
  if (!_set_active(active, value)) return _value;
  if (!_is_tracking && abs(value - _init_value) < kTreshold) return _value;
  _is_tracking = true;
  _value = value;
  return _value;
};

bool MValue::_set_active(const bool active, const float value) {
    if (active && !_is_active) {
      _init_value = value;
    }
    else if (_is_active && !active) {
      _is_tracking = false;
    }
    _is_active = active;
    return active;
  }