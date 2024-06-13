#include "bass.h"
#include <functional>

using namespace synthux;

Bass::Bass():
_trigger            { Trigger(kPPQN) },
_dice               { std::uniform_int_distribution<uint8_t>(0, 100) },
_tempo              { .45f },
_env                { 0.f },
_human_env_kof      { 0.f },
_random_note_chance { 0 },
_human_env_chance   { 0 },
_scale_index        { 0 },
_is_arp_on          { false },
_is_latched         { false } 
{
    _reverb_in.fill(0);
    _reverb_out.fill(0);
    _bus.fill(0);
    _hold.fill(false);
};
  
void Bass::Init(const float sample_rate, const float buffer_size) {
    using namespace std::placeholders;

    _clock.Init(1e6 * buffer_size / sample_rate, kPPQNExtern, kPPQN);

    auto on_clock = std::bind(&Bass::_on_clock_tick, this);
    _clock.SetOnTick(on_clock);

    auto on_arp_note_on = std::bind(&Bass::_on_arp_note_on, this, _1, _2);
    auto on_arp_note_off = std::bind(&Bass::_on_arp_note_off, this, _1);
    _arp.SetOnNoteOn(on_arp_note_on);
    _arp.SetOnNoteOff(on_arp_note_off);
    _arp.SetDirection(kArpPlayDown ? ArpDirection::rev : ArpDirection::fwd);
    _arp.SetRandChance(0);
    _arp.SetAsPlayed(kArpAsPlayed);
    _arp.SetNonLegato(kArpNonLegato);

    auto on_driver_note_on = std::bind(&Bass::_on_driver_note_on, this, _1, _2, _3);
    auto on_driver_note_off = std::bind(&Bass::_on_driver_note_off, this, _1);
    _driver.SetOnNoteOn(on_driver_note_on);
    _driver.SetOnNoteOff(on_driver_note_off);


    Envelope::Config env_cfg;
    env_cfg.time_range = kEnvelopeTimeRange;
    env_cfg.attack_min = kEnvelopeMinAttack;
    env_cfg.decay_a_min = kEnvelopeMinDecayA;
    env_cfg.decay_b_min = kEnvelopeMinDecayB;

    Vox::Config vox_cfg;
    vox_cfg.osc1_waveform_a = kOsc1WaveformA;
    vox_cfg.osc1_waveform_b = kOsc1WaveformB;
    vox_cfg.osc2_waveform = kOsc2Waveform;
    vox_cfg.env_cfg = env_cfg;

    for (auto& v: _voices) v.Init(sample_rate, vox_cfg);

    Filter::Config flt_cfg;
    flt_cfg.freq_min = kFilterFreqMin;
    flt_cfg.freq_max = kFilterFreqMax;
    flt_cfg.env_cfg = env_cfg;

    _filter.Init(sample_rate, flt_cfg);

    _reverb.Init(sample_rate);
    _reverb.SetFeedback(kReverbFeedback);
    _reverb.SetLpFreq(kReverLPFreq);
};

void Bass::SetArpOn(const bool value) {
    if (value != _is_arp_on) Reset();
    _is_arp_on = value;
    auto envelope_mode = _is_arp_on && !kArpNonLegato ? Envelope::Mode::AR : Envelope::Mode::ASR;
    for (auto& v: _voices) v.SetEnvelopeMode(envelope_mode);
    _filter.SetEnvelopeMode(envelope_mode);
};

void Bass::SetLatch(const bool latch) {
    if (_is_latched && !latch) {
        for (uint8_t i = 0; i < _hold.size(); i++) {
        if (_hold[i]) {
            _arp.NoteOff(i);
            _hold[i] = false;
        }
        }
        if (!_arp.HasNote()) Reset();
    }
    _is_latched = latch;
};

void Bass::NoteOn(const uint8_t note) {
    if (!_is_arp_on) {
        _driver.NoteOn(note);
        return;
    }

    if (_is_latched && _hold[note]) {
        _arp.NoteOff(note);
        _hold[note] = false;
    }
    else {
        _arp.NoteOn(note, 127);
        _hold[note] = true;
    }

    if (_arp.HasNote()) {
        if (!_clock.IsRunning()) _clock.Run();
    }
    else {
        Reset();
    }
};

void Bass::NoteOff(const uint8_t note) {
    if (!_is_arp_on) {
        _driver.NoteOff(note);
        _hold[note] = false;
        return;
    }
    if (!_is_latched) {
        _arp.NoteOff(note);
        _hold[note] = false;
    }
    if (!_arp.HasNote()) {
        Reset();
    }
};

void Bass::AllNotesOff() { 
    _arp.Clear();
    _driver.AllOff();
};

void Bass::Reset() {
    _clock.Stop();
    _trigger.Reset();
    _pattern.Reset();
    AllNotesOff();
};

void Bass::SetVoxParams(const VoxParams& p) {
    auto osc1_mult = _scale.TransMult(p.osc1_pitch);

    auto osc2_mult = 1.f;
    Vox::Osc2Mode mode;
    switch (p.osc2_mode_index) {
        case 0: 
        mode = Vox::Osc2Mode::sound; 
        osc2_mult = _scale.TransMult(p.osc2_pitch);
        break;
        
        default: 
        mode = Vox::Osc2Mode::am;  
        osc2_mult = 99 * p.osc2_pitch * p.osc2_pitch;
        break;
    }

    _env = p.env;

    for (auto& v: _voices) {
        v.SetOsc1Shape(p.osc1_shape);
        v.SetOsc1Mult(osc1_mult);
        v.SetOsc2Mult(osc2_mult);
        v.SetOsc2Amount(p.osc2_amnt);
        v.SetOsc2Mode(mode);
    }
};

void Bass::SetFilterParams(const FilterParams& p) {
    _filter.SetFreq(p.freq);
    _filter.SetReso(p.reso);
    _filter.SetEnvelopeAmount(p.env_amount);
};

void Bass::Process(float **out, size_t size) {
    _clock.Tick();
    float output;
    for (size_t i = 0; i < size; i++) {
        output = 0;
        for (auto k = 0; k < kVoxCount; k++) {
        output += _voices[k].Process() * .5f;
        }
        _bus[0] = _bus[1] = _filter.Process(output);
        _xfade.Process(0, 0, _bus[0], _bus[1], _reverb_in[0], _reverb_in[1]);
        _reverb.Process(_reverb_in[0], _reverb_in[1], &(_reverb_out[0]), &(_reverb_out[1]));
        out[0][i] = (_bus[0] + _reverb_out[0]) * .75f;
        out[1][i] = (_bus[1] + _reverb_out[1]) * .75f;
    }
};

void Bass::_on_clock_tick() { 
    if (_trigger.Tick() && _pattern.Tick()) {
        _arp.Trigger();
    }
};

void Bass::_on_arp_note_on(uint8_t num, uint8_t vel) { 
    _driver.NoteOn(num);
};

void Bass::_on_arp_note_off(uint8_t num) {
    _driver.NoteOff(num);
};

void Bass::_on_driver_note_on(uint8_t vox_idx, uint8_t num, bool retrigger) {
    auto h_env = _is_arp_on ? _humanized_envelope(_env, _pattern.Length()) : _env;
    auto freq = _scale.FreqAt(num, _is_arp_on ? _random_note_chance : 0);
    auto& v = _voices[vox_idx];
    _filter.SetEnvelope(h_env);
    _filter.Trigger(retrigger);
    v.SetEnvelope(h_env);
    v.NoteOn(freq, 1.f, retrigger);
};

void Bass::_on_driver_note_off(uint8_t vox_idx) {
    _voices[vox_idx].NoteOff();
    if (!_driver.HasNotes()) _filter.Release();
};

float Bass::_humanized_envelope(float env, uint8_t length) {
    if (_human_env_chance <= 2) return env;
    auto human_env_chance_dice = _dice(_rand_engine);
    if (human_env_chance_dice >= _human_env_chance) return env;
    auto delta = static_cast<float>(_dice(_rand_engine) - 50);
    if (delta > 0) {
        delta *= _human_env_kof * length;
    }
    else { 
        delta *= _human_env_kof;
    }
    return std::clamp(env + delta, 0.f, 1.f); 
};
