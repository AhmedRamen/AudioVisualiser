#include <SDL.h>
#include <SDL_ttf.h>
#undef main //Needed because this caused errors without it
#include "Components/ui.h"
#include "Components/audio.h"

using namespace AV;

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) == -1) {
		panic_and_abort("SDL_Init failed", SDL_GetError());
	}

	bool running = true;
	Window window;
	UI ui;

	//This breaks audio if removed lol
	SDL_zero(desired);
	desired.freq = 48000;
	desired.format = AUDIO_F32;
	desired.channels = 2;
	desired.samples = 2048;
	desired.callback = NULL;
	
	//Attempt to open up driver
	audio_device = SDL_OpenAudioDevice(NULL, 0, &desired, NULL, 0);
	//Couldn't open the audio device? Throw error.
	if (audio_device == 0) {
		panic_and_abort("Couldn't open audio device.", SDL_GetError());
	}

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE); //tells SDL that the event is disabled by default.
	double speed = 0;

	const int targetFPS = 60;
	const int frameDelay = 1000 / targetFPS; // Delay in milliseconds

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

		AudioStreamUpdate();

		window.Render();

		ui.idk.setRotation(speed);
		ui.Render(window.GetRenderer());
		if (wavbuf != NULL)
			drawWaveform(renderer, samplePointer, *num_samplePointer);
		window.Update();


		// Delay to maintain target FPS
		Uint32 frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}

	//Clean up

	SDL_FreeWAV(wavbuf);

	SDL_CloseAudioDevice(audio_device);

	window.~Window();

	TTF_Quit();

	SDL_Quit();

	return 0;
}