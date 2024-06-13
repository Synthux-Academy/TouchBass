#pragma once

#include "../nocopy.h"
#include <array>
#include <math.h>

namespace synthux {

/**
 * @brief
 * Pattern generator
 * Generates equaly distributed patterns of 1...16 onset.
 * This produces both "regular" and "euclidean" patterns.
 */
class CPattern {
public:
    CPattern();
    ~CPattern() {}

    void SetShift(const float frac_offset) {
        // 0...half of the pattern
        _shift = static_cast<uint8_t>(std::roundf(frac_offset * kSize));
    }
    void SetMaxOnsets(uint8_t max_onsets) {
        //0...16
        _max_onsets = std::min(max_onsets, kSize);
    }
    void SetOnsets(const float frac_onsets);

    bool Tick();

    uint8_t Length() const { return _current_length; }

    void Reset() { _next_point = 0; }

private:
    NOCOPY(CPattern)

    static constexpr uint8_t kSize = 16;

    std::array<uint8_t, kSize> _pattern;
    std::array<uint8_t, kSize> _length;
    uint8_t _current_length;
    uint8_t _onsets;
    uint8_t _max_onsets;
    uint8_t _next_point;
    uint8_t _shift;
};

};
