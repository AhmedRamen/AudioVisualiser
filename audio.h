#pragma once
#include <SDL.h>

static SDL_AudioDeviceID audio_device = 0;

SDL_AudioSpec desired;

bool paused = true;

bool OpenAudioFile(const char* fname, SDL_Window* window);

inline void PauseMusic() {
	paused = paused ? false : true;
	SDL_PauseAudioDevice(audio_device, paused);
}

//Volume
float volume_slider_value = 1.0f; 

Uint8* wavbuf = NULL;
Uint32 wavlen = 0;
SDL_AudioSpec wavspec;
SDL_AudioStream* stream = NULL;

//SDL_Window* window is not really needed to open an audio file, but because of screen.h, there's no actual global window variable.
bool OpenAudioFile(const char *fname, SDL_Window* window) {
	//wav variables
	SDL_FreeAudioStream(stream);
	stream = NULL;
	SDL_FreeWAV(wavbuf);
	wavbuf = NULL;
	wavlen = 0;

	//Check if file exists
	if (SDL_LoadWAV(fname, &wavspec, &wavbuf, &wavlen) == NULL) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Wav file loading failed!", SDL_GetError(), window);
		//Clear the previous WAV.
		SDL_FreeWAV(wavbuf);
		wavbuf = NULL;
		wavlen = 0;
		return false;
	}
	
	//Create stream (if we can that is)
	stream = SDL_NewAudioStream(wavspec.format, wavspec.channels, wavspec.freq, AUDIO_F32, 2, 48000);
	if (!stream) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Couldn't create audio stream!", SDL_GetError(), window);
		//Clear the previous WAV.
		SDL_FreeWAV(wavbuf);
		wavbuf = NULL;
		wavlen = 0;
		return false;
	}

	//Stream is dogshit??
	if (SDL_AudioStreamPut(stream, wavbuf, wavlen) == -1) {
		panic_and_abort("Audio stream put failed", SDL_GetError());
	}

	SDL_AudioStreamFlush(stream); //Error handling should be fixed.

	return true;
}
