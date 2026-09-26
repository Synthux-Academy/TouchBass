#pragma once

#include "daisy_seed.h"
#include "../touch/touch.h"
#include "../bass/bass.h"
#include "../config.h"
#include "mvalue.h"
#include <functional>

namespace synthux { 

class BassUI {
public:
    BassUI(Touch& touch, Bass& bass):
    _touch { touch },
    _bass { bass },
    _scale_index  { 0 },
    _suppressed_pads { 0 },
    _blink_frames { 0 }
     {}

    ~BassUI() {}

    void Init(daisy::DaisySeed& hw);
    void Process(daisy::DaisySeed& hw);

private:
    bool _next_scale() {
        _scale_index ++;
        _scale_index = std::min(static_cast<uint8_t>(kScales.size() - 1), _scale_index);
        _bass.SetRandomNoteScaleIndex(_scale_index);
        return _scale_index == kScales.size() - 1;
    }

    bool _prev_scale() {
        if (_scale_index > 0) _scale_index--;
        _bass.SetRandomNoteScaleIndex(_scale_index);
        return _scale_index == 0;
    }

    void _blink() { _blink_frames = kBlinkCount * 2 * kBlinkFrames; }

    void _on_pad_touch(uint16_t pad);
    void _on_pad_release(uint16_t pad);
    
    #ifdef USB_MIDI
    daisy::MidiUsbHandler _midi;
    void _process_midi();
    #endif

    Touch& _touch;
    Bass& _bass;

    MValue _flt_freq;
    MValue _flt_reso;
    MValue _flt_env_amount;
    MValue _osc_1_freq;
    MValue _osc_1_shape;
    MValue _osc_2_freq;
    MValue _osc_2_amount;
    MValue _pattern_value;
    MValue _human_note_value;
    MValue _human_env_value;
    MValue _verb_value;
    MValue _env_value;

    static constexpr uint8_t kNotesCount = 7;
    static constexpr uint16_t kFirstNotePad = 3;
    static constexpr uint16_t kBlinkFrames = 20; // ~90 ms per half cycle
    static constexpr uint16_t kBlinkCount = 3;

    uint8_t _scale_index;
    uint16_t _suppressed_pads;
    uint16_t _blink_frames;
    bool _is_to_touched;
    bool _is_ch_touched;
};

};
