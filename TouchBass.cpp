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


void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
	bass.Process(out, size);
};

int main(void) {
	hw.Init();
	hw.SetAudioBlockSize(4);
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	HW::hw().setHW(&hw);
	HW::hw().startLog();

	touch.Init(hw);
	bass.Init(hw.AudioSampleRate(), hw.AudioBlockSize());
	ui.Init(hw);

	hw.StartAudio(AudioCallback);

	while(1) {
		ui.Process(hw);
		System::Delay(4);
	}
};
