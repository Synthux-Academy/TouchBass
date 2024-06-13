#include "env.h"

using namespace synthux;

Envelope::Envelope():
  _out { 0.f },
  _curve_kof { .5f },
  _t_min_attack { 0.f },
  _t_min_decay_a { 0.f },
  _t_min_decay_b { 0.f },
  _t_range_2x { 0.f },
  _t_reset_out { 0.f },
  _t_reset { 0.f },
  _t_reset_kof { 0.f },
  _t_attack_kof { 0.f },
  _t_decay_kof { 0.f },
  _t_attack { 0 },
  _t_decay { 0 },
  _phase { 0 },
  _mode { Mode::AR },
  _stage { Stage::idle }
  {};

void Envelope::Init(const float sample_rate, const Config cfg) {
    _t_range_2x = 2.0 * cfg.time_range * sample_rate; // Multiplication by 2 is done for optimisation
    _t_min_attack = cfg.attack_min * sample_rate;
    _t_min_decay_a = cfg.decay_a_min * sample_rate;
    _t_min_decay_b = cfg.decay_b_min * sample_rate;
    _t_reset = .008f * sample_rate; // 8 ms
    _t_reset_kof = 1.f / _t_reset;
};

void Envelope::SetShape(const float value) {
    float curve;
    if (value < 0.5) {
        _t_attack = _t_min_attack;
        _t_decay = static_cast<size_t>(_t_range_2x * value * value) + _t_min_decay_a;
        curve = value * .15f;
    }
    else {
        auto norm_val = 2.f * value - 1.f;
        _t_attack = static_cast<size_t>(_t_range_2x * norm_val * norm_val) + _t_min_attack;
        _t_decay = static_cast<size_t>(_t_range_2x * (1.f - value * value)) + _t_min_decay_b;
        curve = .5f;
    }
    _t_attack_kof = 1.f / _t_attack;
    _t_decay_kof = 1.f / _t_decay;
    _set_curve(curve);
};

void Envelope::Trigger() {
    switch (_stage) {
    case Stage::idle:
        _stage = Stage::attack;
        _phase = 0;
        break;

    case Stage::decay:
        _stage = Stage::attack;
        _phase = _ph_attack(_out);
        break;

    default: break;
    }
};

void Envelope::Release() {
    switch (_stage) {
        case Stage::attack:
        _phase = _ph_decay(_out);
        _stage = Stage::decay;
        break;

        case Stage::sustain:
        _phase = 0;
        _stage = Stage::decay;
        break;

        default: break;
    }
};

float Envelope::Process() {
    float ph;
    switch (_stage) {
        case Stage::idle: 
        _out = 0.f;
        break;

        case Stage::attack: 
        ph = static_cast<float>(_phase) * _t_attack_kof;
        _out = _amp_attack(ph);
        if (_phase >= _t_attack) {
            _stage = _mode == Mode::AR ? Stage::decay : Stage::sustain;
            _phase = 0;
        }
        else {
            _phase ++;
        }
        break;

        case Stage::sustain:
        _out = 1.f;
        break;

        case Stage::decay:
        ph = static_cast<float>(_phase) * _t_decay_kof;
        _out = _amp_decay(ph);
        if (_phase >= _t_decay) {
            _stage = Stage::idle;
            _phase = 0;
        }
        else {
            _phase ++;
        }
        break;

        case Stage::reset:
        _out = _t_reset_out - static_cast<float>(_phase) * _t_reset_kof;
        if (_phase >= _t_reset) {
            _stage = Stage::idle;
            _phase = 0;
        }
        else {
            _phase ++;
        }
        break;
    }
    return std::min(std::max(_out, 0.f), 1.f);
};

void Envelope::Reset() {
    if (_stage == Stage::idle) return;
    _stage = Stage::reset;
    _t_reset_out = _out;
    _phase = 0;
};
