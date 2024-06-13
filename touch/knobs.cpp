#include "knobs.h"

using namespace synthux;
using namespace daisy;

void Knobs::Init(DaisySeed& hw) {
    auto knob_count = 8;

    AdcChannelConfig cfg[knob_count];
	cfg[0].InitSingle(daisy::seed::A0);
	cfg[1].InitSingle(daisy::seed::A1);
    cfg[2].InitSingle(daisy::seed::A2);
    cfg[3].InitSingle(daisy::seed::A3);
    cfg[4].InitSingle(daisy::seed::A4);
    cfg[5].InitSingle(daisy::seed::A5);
    cfg[6].InitSingle(daisy::seed::A6);
    cfg[7].InitSingle(daisy::seed::A7);
	hw.adc.Init(cfg, knob_count);

    for (auto i = 0; i < knob_count; i++)  {
        _knobs[i].Init(hw.adc.GetPtr(i), hw.AudioCallbackRate());
    }
};