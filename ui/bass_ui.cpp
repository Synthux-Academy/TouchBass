#include "bass_ui.h"
#include "daisysp.h"
#include "../config.h"

using namespace synthux;
using namespace daisy;
using namespace daisysp;

float norm(const uint8_t value) {
    return static_cast<float>(value) / 127.f;
};

 void BassUI::Init(daisy::DaisySeed& hw) {
    // Assign callbacks ////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    using namespace std::placeholders;
    auto on_touch = std::bind(&BassUI::_on_pad_touch, this, _1);
    auto on_release = std::bind(&BassUI::_on_pad_release, this, _1);
    _touch.pads().SetOnTouch(on_touch);
    _touch.pads().SetOnRelease(on_release);

    // Initial preset ///////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    _flt_freq.Set(.1f);
    _flt_reso.Set(.5f);
    _flt_env_amount.Set(.3f);
    _osc_1_freq.Set(1.0f);
    _osc_2_freq.Set(0.5f);
    _osc_2_amount.Set(1.f);
    _pattern_value.Set(0.75f);
    _env_value.Set(0.1f);
    _human_env_value.Set(.25f);
    _verb_value.Set(.2f);

    // Initialize MIDI //////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    #ifdef USB_MIDI
    daisy::MidiUsbHandler::Config midi_cfg;
    _midi.Init(midi_cfg);
    #endif
};

void BassUI::Process(DaisySeed& hw) {
    #ifdef USB_MIDI
    _process_midi();
    #endif

    // Process touch ////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    _touch.Process();
    _is_to_touched = _touch.pads().IsTouched(10);
    _is_ch_touched = _touch.pads().IsTouched(11);

    // Arp mode /////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    auto arp_mode_value = _touch.switches().A();
    _bass.SetArpOn(arp_mode_value != daisy::Switch3::POS_DOWN);
    _bass.SetLatch(arp_mode_value == daisy::Switch3::POS_UP);

    // Voice parameters /////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    Bass::VoxParams v_params;
    auto osc1_value = _touch.knobs().s31().Process();
    v_params.osc1_pitch = _osc_1_freq.Process(osc1_value, !_is_to_touched);
    v_params.osc1_shape = _osc_1_shape.Process(osc1_value, _is_to_touched);
    v_params.osc2_pitch = _touch.knobs().s32().Process();
    auto osc2_mode_switch = _touch.switches().B();
    switch (osc2_mode_switch) {
        case Switch3::POS_CENTER: v_params.osc2_mode_index = 1; break;
        case Switch3::POS_UP: v_params.osc2_mode_index = 2; break;
        default: v_params.osc2_mode_index = 0;
    }
    auto osc2_amnt = fmap(_touch.knobs().s30().Process(), 0.f, 1.f, Mapping::EXP);
    v_params.osc2_amnt = _osc_2_amount.Process(osc2_amnt, true);

    auto env = _touch.knobs().s37().Process();
    v_params.env = _env_value.Process(env, true);
    _bass.SetVoxParams(v_params);

    auto pattern = _touch.knobs().s33().Process();
    _bass.SetPattern(_pattern_value.Process(pattern, true));

    auto human_note = _touch.knobs().s34().Process();
    _bass.SetRandomNoteChance(_human_note_value.Process(human_note, true));

    // Filter parameters ////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    Bass::FilterParams f_params;
    auto flt_value = fmap(_touch.knobs().s36().Process(), 0.f, 1.f, Mapping::EXP);
    f_params.freq = _flt_freq.Process(flt_value, !_is_to_touched && !_is_ch_touched);
    f_params.reso = _flt_reso.Process(flt_value, _is_to_touched);
    f_params.env_amount = _flt_env_amount.Process(flt_value, _is_ch_touched);
    _bass.SetFilterParams(f_params);

    // Human envelope / Reverb //////////////////////////////////
    /////////////////////////////////////////////////////////////
    auto human_verb_knob_value = _touch.knobs().s35().Process();
    _bass.SetHumanEnvelopeChance(_human_env_value.Process(human_verb_knob_value, !_is_to_touched && !_is_ch_touched));
    _bass.SetReverbMix(_verb_value.Process(human_verb_knob_value, _is_to_touched));  

    hw.SetLed(_bass.IsLatched());
};

void BassUI::_on_pad_touch(uint16_t pad) {
    // Scale & Tempo
    if (pad == 0) {
        if (_is_to_touched) _bass.SlowDown();
        else if (_is_ch_touched) _prev_scale();
        return;
    }
    if (pad == 2) {
        if (_is_to_touched) _bass.SpeedUp();
        else if (_is_ch_touched) _next_scale();
        return;
    }

    // Mono / para
    if (pad == 11 && _is_to_touched) {
        if (_bass.IsMono()) _bass.SetPoly();
        else _bass.SetMono();
    }

    // Notes
    if (pad < kFirstNotePad || pad >= kFirstNotePad + kNotesCount) return;
    auto note = kScales[_scale_index][pad - kFirstNotePad];
    _bass.NoteOn(note);
};

void BassUI::_on_pad_release(uint16_t pad) {
    if (pad < kFirstNotePad || pad >= kFirstNotePad + kNotesCount) return;
    auto note = kScales[_scale_index][pad - kFirstNotePad];
    _bass.NoteOff(note);
};

#ifdef USB_MIDI
void BassUI::_process_midi() {
    _bass.ProcessClockIn(false);
    _midi.Listen();
    while(_midi.HasEvents()) {
        auto msg = _midi.PopEvent();
        switch(msg.type) {
            case SystemRealTime: {
                switch (msg.srt_type) {
                    case TimingClock: {
                        _bass.ProcessClockIn(true);
                    }
                    break;
                    default: break;
                }
            }
            break;
            case NoteOn: {
                auto note_msg = msg.AsNoteOn();
                _bass.NoteOn(note_msg.note);
            }
            break;
            case NoteOff: {
                auto note_msg = msg.AsNoteOff();
                _bass.NoteOff(note_msg.note);
            }
            case ControlChange: {
                auto ctrl_msg = msg.AsControlChange();
                auto num = ctrl_msg.control_number;
                auto norm_value = norm(ctrl_msg.value);
                if (num == 71) { _flt_reso.Set(norm_value); }              // resonance
                else if (num == 72) { _env_value.Set(norm_value); }             // envelope
                else if (num == 74) { _flt_freq.Set(norm_value); }         // cut off
                else if (num == 75) { _osc_1_freq.Set(norm_value); }       // osc 1 freq
                else if (num == 76) { _osc_1_shape.Set(norm_value); }      // osc 1 shape
                else if (num == 77) { _osc_2_freq.Set(norm_value); }       // osc 2 freq
                else if (num == 78) { _osc_2_amount.Set(norm_value); }     // osc 2 amount
                else if (num == 85) { _pattern_value.Set(norm_value); }    // pattern
                else if (num == 86) { _human_note_value.Set(norm_value); } // human note
                else if (num == 87) { _human_env_value.Set(norm_value); }  // human envelope
                else if (num == 91) { _verb_value.Set(norm_value); }       // reverb
                else if (num == 123) { _bass.Reset(); }
                else if (num == 126) { _bass.SetMono(); }
                else if (num == 127) { _bass.SetPoly(); }
            }
            break;
            
            default: break;
        }
    }
};
#endif