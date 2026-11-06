#include "daisy_seed.h"
#include "touch/touch.h"
#include "bass/bass.h"
#include "ui/bass_ui.h"
#include "log.h"

using namespace daisy;
using namespace synthux;

DaisySeed hw;

Touch touch;
Bass bass;
BassUI ui(touch, bass);

// Analog clock in. Mono jack: tip -> this pin, sleeve -> ground. Change pin here.
static constexpr auto kClockInPin = seed::D28; // S43 on the Simple PCB
GPIO clock_in;
bool clock_in_last = false;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
	// Polled here rather than in the main loop so short trigger pulses aren't missed.
	auto clock_in_state = clock_in.Read();
	if (clock_in_state && !clock_in_last) {
		bass.ProcessClockIn(true);
		bass.ProcessClockIn(false);
	}
	clock_in_last = clock_in_state;

	bass.Process(out, size);
};

int main(void) {
	hw.Init();
	hw.SetAudioBlockSize(4);
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	HW::hw().setHW(&hw);
#ifndef USB_MIDI
	HW::hw().startLog();
#endif

	touch.Init(hw);
	bass.Init(hw.AudioSampleRate(), hw.AudioBlockSize());
	ui.Init(hw);
	clock_in.Init(kClockInPin, GPIO::Mode::INPUT, GPIO::Pull::PULLDOWN);

	hw.StartAudio(AudioCallback);

	while(1) {
		ui.Process(hw);
		System::Delay(4);
	}
};
