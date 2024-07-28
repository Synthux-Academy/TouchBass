#pragma once

#include <array>
#include <stdint.h>
#include "daisysp.h"

// SCALE ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
static constexpr uint8_t kScaleSize = 7;
static constexpr uint8_t kScalesCount = 3;
static constexpr std::array<std::array<uint8_t, kScaleSize>, kScalesCount> kScales = {{
    { 48, 55, 58, 60, 62, 63, 65 }, // Amara C3, G3, A#3, C4, D4, D#4, F4
    { 48, 52, 53, 55, 57, 60, 64 }, // Oxalis C3, E3, F3, G3, A3, C4, E4
    { 48, 50, 51, 55, 58, 60, 62 }  // Pigmy C3, D3, D#3, G3, Bb3, C4, D4
}};

// ARP /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
static constexpr bool kArpNonLegato = false; // Whether it's holding the note until the next one played
static constexpr bool kArpAsPlayed = true;
static constexpr bool kArpPlayDown = false;
static constexpr bool kArpTogglingLatch = false; //If true, each pad/key works as a toggle

// VOX /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
static constexpr int kOsc1WaveformA = daisysp::Oscillator::WAVE_POLYBLEP_SAW;
static constexpr int kOsc1WaveformB = daisysp::Oscillator::WAVE_POLYBLEP_SQUARE;
static constexpr int kOsc2Waveform = daisysp::Oscillator::WAVE_TRI;

// FILTER //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
static constexpr float kFilterFreqMin = 40.f; //Hz
static constexpr float kFilterFreqMax = 10000.f; //Hz

// ENVELOPE ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// The max attack time is kEnvelopeMinAttack + kEnvelopeTimeRange //////////
// The max decay time is kEnvelopeMinDecay(A/B) + kEnvelopeTimeRange ///////
static constexpr float kEnvelopeTimeRange = 4.f; // 4s.
static constexpr float kEnvelopeMinAttack = .01f; // 20 ms
static constexpr float kEnvelopeMinDecayA = .4f; // 400 ms. Fader 0...0.5
static constexpr float kEnvelopeMinDecayB = .01f; // 10 ms. Fader 0.5...1.0

// REVERB //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
static constexpr float kReverbFeedback = .8f;
static constexpr float kReverLPFreq = 10000.f; //Hz

// MIDI ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//#define USB_MIDI