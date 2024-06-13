#include "flt.h"

using namespace synthux;

Filter::Filter(): 
_freq       { 0.f },
_env_amount { 1.f }
{};

void Filter::Init(const float sample_rate, const Config cfg) {
    _cfg = cfg;
    _env.Init(sample_rate, cfg.env_cfg);
    _flt.Init(sample_rate);
    _flt.SetFreq(10000.f);
    _flt.SetRes(0.2f);
};

void Filter::Trigger(bool retrigger) {
    if (retrigger) {
        _env.Reset();
        _pending_retrigger = true;
    }
    else {
        _env.Trigger();
    }
};

float Filter::Process(const float in) {
    auto env = _env.Process() * _env_amount;
    if (_env.IsRunning()) {
        auto freq = std::min(_freq + _freq_env_room * env, _cfg.freq_max);
        _flt.SetFreq(freq);
        _flt.Process(in);
    }
    else if (_pending_retrigger) {
        _pending_retrigger = false;
        _env.Trigger();
    }
    return _flt.Low();
}