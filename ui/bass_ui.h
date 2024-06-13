#pragma once

#include "daisy_seed.h"
#include "../touch/touch.h"
#include "../bass/bass.h"
#include "../config.h"
#include "mvalue.h"
#include <functional>

#ifdef __cplusplus

namespace synthux { 

class BassUI {
public:
    BassUI(Touch& touch, Bass& bass):
    _touch { touch },
    _bass { bass },
    _scale_index  { 0 }
     {}

    ~BassUI() {}

    void Init(daisy::DaisySeed& hw);
    void Process(daisy::DaisySeed& hw);

private:
    void _next_scale() {
        _scale_index ++;
        _scale_index = std::min(static_cast<uint8_t>(_scales.size() - 1), _scale_index);
        _bass.SetRandomNoteScaleIndex(_scale_index);
    }

    void _prev_scale() {
        _scale_index --;
        _scale_index = std::max(static_cast<uint8_t>(0), _scale_index);
        _bass.SetRandomNoteScaleIndex(_scale_index);
    }

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

    uint8_t _scale_index;

    bool _is_to_touched;
    bool _is_ch_touched;

    static constexpr uint8_t kNotesCount = 7;
    static constexpr uint16_t kFirstNotePad = 3;

    std::array<std::array<uint8_t, kNotesCount>, 3> _scales = {{
        { 48, 55, 58, 60, 62, 63, 65 }, // Amara C3, G3, A#3, C4, D4, D#4, F4
        { 48, 52, 53, 55, 57, 60, 64 }, // Oxalis C3, E3, F3, G3, A3, C4, E4
        { 48, 50, 51, 55, 58, 60, 62 }  // Pigmy C3, D3, D#3, G3, Bb3, C4, D4
    }};
};

};

#endif
