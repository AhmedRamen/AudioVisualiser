#include <SDL.h>
#undef main //Needed because this caused errors without it
#include "screen.h"
#include "audio.h"
#include "ui.h"

int main() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
		panic_and_abort("SDL_Init failed", SDL_GetError());
	}
	
	bool running = true;
	Window window;
	UI ui;

	SDL_zero(desired);
	desired.freq = 48000;
	desired.format = AUDIO_F32;
	desired.channels = 2;
	desired.samples = 4096;
	desired.callback = NULL;

	//Attempt to open up driver
	audio_device = SDL_OpenAudioDevice(NULL, 0, &desired, NULL, 0);
	//Couldn't open the audio device? Throw error.
	if (audio_device == 0) {
		panic_and_abort("Couldn't open audio device.", SDL_GetError());
	}

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE); //tells SDL that the event is disabled by default.

	//OpenAudioFile("richfresh.wav", window.getWindow());
		
	//Update frame until we close program
	while (running) {
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

		AudioStreamUpdate();

		window.Render();
		ui.Render(window.GetRenderer());
		window.Update();

	}

	SDL_CloseAudioDevice(audio_device);

	window.~Window();

	SDL_Quit();

	return 0;
}