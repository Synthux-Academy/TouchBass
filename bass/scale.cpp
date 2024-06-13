#include "scale.h"

using namespace synthux;

Scale::Scale():
_trans_index  { 12 },
_scale_index  { 0 },
_dice { std::uniform_int_distribution<uint8_t>(0, 100) },
_note_distribution { std::binomial_distribution<uint8_t>(kScaleSize - 1, .5f) }
{ };

float Scale::FreqAt(uint8_t note, const uint8_t random_note_chance) {
  if (random_note_chance > 2) { 
    auto human_note_chance_dice = _dice(_rand_engine);
    auto octave_dice = _dice(_rand_engine);
    if (random_note_chance < 45) {
      if (human_note_chance_dice < random_note_chance) {
        note += (octave_dice < 50) ? -12 : 12;
      }
    }
    else if (random_note_chance < 90) {
      if (human_note_chance_dice < random_note_chance) {
        if (octave_dice < 25) note -= 12;
        else if (octave_dice > 75) note += 12; 
      }
    }
    else {
      note = _randomise(note);
      if (octave_dice < 5) note += 24;
      else if (octave_dice < 20) note += 12;
      else if (octave_dice > 80) note -= 12;
      else if (octave_dice > 95) note -= 24;
      //60% of freq passes through unchanged
    }
  }
  return daisysp::mtof(note);
};
