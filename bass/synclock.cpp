#include "synclock.h"
#include <stdint.h>

using namespace synthux;

SynClock::SynClock():
_on_tick             { nullptr },
_manual_tempo        { 120 },
_raw_manual_tempo    { 120 },
_ppqn_out            { 48 },
_tr_time             { 0 },
_ticks_per_clock     { 1 },
_ticks               { 0 },
_fticks              { 0 },
_ticks_at_last_clock { 0 },
_tempo_ticks         { 0 },
_tempo_mks           { 500000 },
_hold                { false },
_resync              { false },
_is_running          { false },
_is_about_to_run     { false },
_last_state          { false }
{};

void SynClock::Init(const float update_interval_mks, const uint32_t ppqn_in, const uint32_t ppqn_out) {
    _tr_time = ppqn_out * update_interval_mks;
    _ticks_per_clock = ppqn_out / ppqn_in;
    _ppqn_out = ppqn_out;
};

void SynClock::Process(const bool state) {
    if (state && !_last_state) {
        _external_clock_tick();
    }
    _last_state = state;
};

void SynClock::SetTempo(const float norm_value) {
    if (fcomp(norm_value, _raw_manual_tempo)) return;
    _raw_manual_tempo = norm_value;
    const auto clock_off_offset = 10;
    _manual_tempo = (kBPMRange - clock_off_offset) * norm_value + kBPMMin - clock_off_offset;
    _tempo_mks = _get_tempo_mks(_manual_tempo);
    if (_external_clock()) {
        if (_is_running) {
            _is_running = false;
            _is_about_to_run = true;
        }
        else if (_is_about_to_run) {
            _is_running = true;
            _is_about_to_run = false;
        }
        _reset();
    }
};

void SynClock::_external_clock_tick() {
    if (!_external_clock()) return;
    if (!_is_running && !_is_about_to_run) return;

    if (_is_about_to_run) {
        _is_about_to_run = false;
        _is_running = true;
    }
    else {
        _resync = true;
        _hold = false;
        _emit_ticks();
    }
};

/*
Derived from Maximum MIDI Programmer's ToolKit Copyright ©1993-1998 by Paul Messick.
This method generates internal ticks and also synchronises to the external clock.
So it's called both from internal interrupt timer (audio callback in this implementation) and upon external clock tick reception.
nticks - integer count of internal ticks
_fticks - fractional count of internal ticks
_tempo_ticks - integer ticks count since last external clock
_tempo_mks - tempo in microseconds / beat (quarter note)
_resync - flag to resync to external clock. Is set to true once external clock tick is received.
_hold - flag to stop advancing internal timeline if the number of internal ticks exceeded expected count of internal ticks per extrnal tick
_tr_time - internal resolution (_ppqn_out) multiplied by interrupt interval.
*/
void SynClock::_emit_ticks() {
    uint32_t nticks = 0;

    //If we generated more internal ticks per extrnal tick as expected,
    //we don't advance internal "timeline", but only accumulate _tempo_ticks
    //in order to calculate and correct the tempo.
    //This flag is set to false upon reception of the external tick.
    if (_hold) {
        nticks = (_fticks + _tr_time) / _tempo_mks;
        _fticks += _tr_time - (nticks * _tempo_mks);
        _tempo_ticks += nticks;
        return;
    }

    //Once a tick of the extrnal clock is received,
    //we do resync, i.e. align inernal timeline with the external one
    //and adjust tempo.
    if (_resync) {
        _fticks = 0;
        nticks = _ticks_per_clock - (_ticks - _ticks_at_last_clock);
        _ticks_at_last_clock = _ticks + nticks;
        _tempo_mks -= (static_cast<int32_t>(_ticks_per_clock) - static_cast<int32_t>(_tempo_ticks)) * static_cast<int32_t>(_tempo_mks) / static_cast<int32_t>(_ppqn_out);
        _tempo_ticks = 0;
        _resync = false;
    }
    //Regular mode. We generate internal ticks.
    else {
        nticks = (_fticks + _tr_time) / _tempo_mks;
        _fticks += _tr_time - nticks * _tempo_mks;
        if (_external_clock()) {
            _tempo_ticks += nticks;
            //If there are more internal ticks per the external tick than 
            //expected, we set _hold to true effectively stopping advancing timeline
            //until next external tick
            if (_ticks - _ticks_at_last_clock + nticks >= _ticks_per_clock) {
                nticks = _ticks_per_clock - 1 - (_ticks - _ticks_at_last_clock);
                _hold = true;
            }
        }
    }

    //Accumulate ticks
    _ticks += nticks;

    //Advance timeline
    if (_on_tick != nullptr) {
        for (uint32_t i = 0; i < nticks; i++) _on_tick();
    }
};

void SynClock::_reset() {
    _fticks = 0;
    _ticks = 0;
    _ticks_at_last_clock = 0;
    _tempo_ticks = 0;
    _hold = false;
    _resync = false;
};
