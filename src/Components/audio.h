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

inline void stop_audio() {
	//Destroy stream
	if (stream) {
		SDL_FreeAudioStream(stream);
	}
	//Destroy the current wav buffer
	if (wavbuf) {
		SDL_FreeWAV(wavbuf);
	}

	stream = NULL;
	wavbuf = NULL;
	wavlen = 0;
}


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
		stop_audio();
	}
	
	//Create stream (if we can that is)
	stream = SDL_NewAudioStream(wavspec.format, wavspec.channels, wavspec.freq, AUDIO_F32, 2, 48000);
	if (!stream) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Couldn't create audio stream!", SDL_GetError(), window);
		stop_audio();
	}

	//Stream is dogshit??
	if (SDL_AudioStreamPut(stream, wavbuf, wavlen) == -1) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Audio stream put failed!", SDL_GetError(), window);
		stop_audio();
	}

	if (SDL_AudioStreamFlush(stream) == -1) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Audio stream flushing failed!", SDL_GetError(), window);
		stop_audio();
	}; //Error handling should be fixed.

	return true;

}


//Audio chunking.
inline void AudioStreamUpdate() {
	if (SDL_GetQueuedAudioSize(audio_device) < 8192) {
		//Get bytes from audio stream
		const int bytes_remaining = SDL_AudioStreamAvailable(stream);
		//No more bytes to accumulate, queue more audio?
		if (bytes_remaining > 0) {
			const Uint32 new_bytes = SDL_min(bytes_remaining, 32 * 1024);
			Uint8 convert_bufferer[32 * 1024];
			const int num_converted_bytes = SDL_AudioStreamGet(stream, convert_bufferer, new_bytes);
			//Convert bytes is more than zero
			if (num_converted_bytes > 0) {
				const int num_samples = (num_converted_bytes / sizeof(float));
				float* samples = (float*)convert_bufferer;

				//Change the volume of the audio
				if (volume_slider_value != 1.0f) {
					for (int i = 0; i < num_samples; i++)
						samples[i] *= volume_slider_value;
				}
			}
			SDL_QueueAudio(audio_device, convert_bufferer, new_bytes);
		}
	}
}