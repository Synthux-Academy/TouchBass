#pragma once

#include <array>
#include <random>
#include "daisysp.h"
#include "../nocopy.h"
#include "../config.h"

namespace synthux {
class Scale {
public:
  Scale();
  ~Scale() {};

  float TransMult(const float value) {
    auto new_trans_index = static_cast<uint8_t>(value * (_trans.size() - 1));
    return _trans[new_trans_index];
  };

  void SetRandomScaleIndex(const uint8_t index) { _scale_index = index; };

  float FreqAt(uint8_t note, const uint8_t human_note_chance);

private:
  NOCOPY(Scale)    

  uint8_t _randomise(uint8_t reference_note) {
    auto note = kScales[_scale_index][_note_distribution(_rand_engine)];
    return static_cast<uint8_t>(std::clamp<int8_t>(note, 0, 127));
  }

  uint8_t _trans_index;
  uint8_t _scale_index;
  std::default_random_engine _rand_engine;
  std::uniform_int_distribution<uint8_t> _dice;
  std::binomial_distribution<uint8_t> _note_distribution;

  std::array<float, 25> _trans = { 
    0.5f, 
        0.52973154717962f, 
        0.56123102415466f,  0.594603557501335f, 
        0.629960524947413f, 0.667419927084995f, 
        0.707106781186527f, 0.749153538438323f, 
        0.793700525984085f, 0.840896415253703f, 
        0.890898718140331f, 0.943874312681689f, 
        1.f, 
        1.0594630943593f,   1.12246204830938f, 
        1.18920711500273f,  1.25992104989489f, 
        1.33483985417006f,  1.41421356237313f,
        1.49830707687673f,  1.58740105196826f, 
        1.6817928305075f,   1.78179743628076f, 
        1.88774862536348f, 
        2.f 
      };
  };
};
