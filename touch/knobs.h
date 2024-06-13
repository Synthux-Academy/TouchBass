#pragma once

#include "daisy_seed.h"
#include "../nocopy.h"
#include <array>

#ifdef __cplusplus

namespace synthux {

class Knobs {
public:
    Knobs() {}
    ~Knobs() {}

    void Init(daisy::DaisySeed& hw);

    daisy::AnalogControl& s30() { return _knobs[0]; };
    daisy::AnalogControl& s31() { return _knobs[1]; };
    daisy::AnalogControl& s32() { return _knobs[2]; };
    daisy::AnalogControl& s33() { return _knobs[3]; };
    daisy::AnalogControl& s34() { return _knobs[4]; };
    daisy::AnalogControl& s35() { return _knobs[5]; };
    daisy::AnalogControl& s36() { return _knobs[6]; };
    daisy::AnalogControl& s37() { return _knobs[7]; };

private:
    NOCOPY(Knobs)

    std::array<daisy::AnalogControl, 8> _knobs;
};

};

#endif
