#pragma once

// #include <stdint.h>
#include <functional>
#include "daisy_seed.h"
#include "knobs.h"
#include "pads.h"
#include "switches.h"
#include "../nocopy.h"

#ifdef __cplusplus

namespace synthux {

class Touch {
public:
    Touch(): _state { 0 } {}
    ~Touch() {}

    void Init(daisy::DaisySeed& hw) {
      _pads.Init(hw);
      _knobs.Init(hw);
      _switches.Init();

      hw.adc.Start();
    }
    void Process() {
      _pads.Process();
    }

    Pads& pads() { return _pads; }

    Knobs& knobs() { return _knobs; }

    Switches& switches() { return _switches; }

private:
    NOCOPY(Touch)

    uint16_t _state;
    daisy::Mpr121I2C _mpr;
    synthux::Knobs _knobs;
    synthux::Pads _pads;
    synthux::Switches _switches;
};

};

#endif
