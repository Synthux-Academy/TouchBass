#include "cpattern.h"
#include <stdint.h>

using namespace synthux;

CPattern::CPattern():
_onsets     { 0 },
_max_onsets { kSize },
_next_point { 0 },
_shift      { 0 }
{ };

void CPattern::SetOnsets(const float frac_onsets) {
    auto onsets { static_cast<uint8_t>(std::roundf(frac_onsets * (_max_onsets - 1)) + 1) };
    if (onsets == _onsets) return;
    _onsets = onsets;

    if (onsets == 0) { 
        _pattern.fill(0); 
        _length.fill(0);
        return; 
    }
    else if (onsets == kSize) { 
        _pattern.fill(1); 
        _length.fill(1);
        return; 
    }

    // Christoffel word algorythm ///////////////////////////////////
    // Derrived from "Creating Rhythms" (ISBN: 9781887187220)
    // by Stefan Hollos and J. Richard Hollos
    auto y = onsets, a = y;
    auto x = kSize - onsets, b = x;
    auto i = 0;
    _pattern[i++] = 1;
    while (a != b) {
        if (a > b) {  _pattern[i] = 1; b += x;  }
        else {  _pattern[i] = 0; a += y; }
        i++;
    }
    _pattern[i++] = 0;

    if (i == kSize) return;
    // In case of relatively non-prime numbers (e.g. 4/16, 8/16 etc.)
    // only part of the slots will be filled, so we need
    // to copy them to fill the entire patterm
    auto offset = i;
    i = 0;
    while (i + offset < kSize) {
        _pattern[i + offset] = _pattern[i];
        i++;
    }
    ///////////////////////////////////////////////////////////////////

    uint8_t count = 1;
    i = kSize;
    _length.fill(0);
    while (--i >= 0) {
        if (_pattern[i]) { _length[i] = count; count = 1; }
        else { count++; }
    }
};

bool CPattern::Tick() {
    // Apply shift        
    auto point = _next_point - _shift;
    if (point < 0) point += kSize;

    // Advance to the next pattern point
    if (++_next_point == kSize) _next_point = 0;

    // Send trigger if pattern point is 1
    _current_length = _length[point];
    return _pattern[point];
};
