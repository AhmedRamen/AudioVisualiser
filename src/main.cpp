#include <SDL.h>
#include <SDL_ttf.h>
#undef main //Needed because this caused errors without it
#include "Components/ui.h"
#include "Components/audio.h"

using namespace AV;

void startup(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) == -1) {
		panic_and_abort("SDL_Init failed", SDL_GetError());
	}

	if (!Sound_Init()) {
		panic_and_abort("Sound_Init failed", Sound_GetError());
	}

	//This breaks audio if removed lol
	SDL_zero(desired);
	desired.freq = 48000;
	desired.format = AUDIO_F32;
	desired.channels = 2;
	desired.samples = 2048;
	desired.callback = feed_audio_device_callback;

	audio_device_spec.format = desired.format;
	audio_device_spec.channels = desired.channels;
	audio_device_spec.rate = desired.freq;

	//Attempt to open up driver
	audio_device = SDL_OpenAudioDevice(NULL, 0, &desired, NULL, 0);
	//Couldn't open the audio device? Throw error.
	if (audio_device == 0) {
		panic_and_abort("Couldn't open audio device.", SDL_GetError());
	}

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE); //tells SDL that the event is disabled by default.

}

int main(int argc, char* argv[]) {

	startup(argc, argv);

	Window window;
	UI ui;

	const int targetFPS = 60;
	const int frameDelay = 1000 / targetFPS; // Delay in milliseconds

	double speed = 0;

	bool running = true;

	//Update frame until we close program
	while (running) {
		Uint32 frameStart = SDL_GetTicks();
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			ui.HandleEvent(e);
			switch (e.type) {
			case SDL_QUIT:
				running = SDL_FALSE;
				break;

			case SDL_DROPFILE:
				OpenAudioFile(e.drop.file, window.getWindow());
				//clean up
				SDL_free(e.drop.file);
				break;
			}
		}
		speed -= 0.5;

		window.Render();

		ui.idk.setRotation(speed);
		//If it exists, then we draw.
		if (current_sample)
			drawWaveform(renderer, window.getWidth(), window.getHeight());

		ui.Render(window.GetRenderer());

		window.Update();


		// Delay to maintain target FPS
		Uint32 frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}

	//Clean up
	SDL_CloseAudioDevice(audio_device);

	if (current_sample) {
		Sound_FreeSample(current_sample);
		current_sample = NULL;
	}

	window.~Window();

	TTF_Quit();

	Sound_Quit();

	SDL_Quit();

	return 0;
}