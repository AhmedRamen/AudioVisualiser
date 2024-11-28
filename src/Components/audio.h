#pragma once
#include <SDL.h>
#include <iostream>
#include <mutex>
#include <vector>
#include <algorithm>
#include "screen.h"

static SDL_AudioDeviceID audio_device = 0;

std::mutex waveform_mutex;

std::vector<float> waveform_data; //Store waveform samples

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


// Function to draw waveform
inline void drawWaveform(SDL_Renderer* renderer, int lines, int width, int height) {
    size_t dataSize;
    {
        std::lock_guard<std::mutex> lock(waveform_mutex);
        dataSize = waveform_data.size();
    }

    // No data exists yet.
    if (dataSize < 2)
        return;

    // Calculate how many samples to skip for each line segment
    size_t cut = dataSize / (lines - 1);

    // Precompute scaling factors
    float heightScale = height / 2.0f;
    float halfHeight = height / 2;

    // Draw lines based on accumulated samples
    for (size_t i = 0; i < lines - 1; ++i) {
        // Get index for first and second sample
        size_t index1 = i * cut; 
        size_t index2 = (i + 1) * cut;

        // Ensure indices are within bounds
        if (index2 >= dataSize) {
            index2 = dataSize - 1;
        }

        float sample1 = waveform_data[index1];
        float sample2 = waveform_data[index2];

        // Scale sample to window's height
        int point_y1 = static_cast<int>((sample1 * heightScale) + halfHeight);
        int point_y2 = static_cast<int>((sample2 * heightScale) + halfHeight);
        //Spread to window's width
        int point_x1 = static_cast<int>((index1 * width) / dataSize);
        int point_x2 = static_cast<int>((index2 * width) / dataSize);

        // Draw line between two points
        SDL_SetRenderDrawColor(renderer, 0, 255, 128, 255);
        SDL_RenderDrawLine(renderer, point_x1, point_y1, point_x2, point_y2);
    }
}

// Audio chunking.
inline void AudioStreamUpdate() {
    if (SDL_GetQueuedAudioSize(audio_device) < 8192) {
        //Get bytes from audio stream
        const int bytes_remaining = SDL_AudioStreamAvailable(stream);

        //No more bytes to accumulate, queue more
        if (bytes_remaining > 0) {
            const Uint32 new_bytes = SDL_min(bytes_remaining, 32 * 1024);
            Uint8 convert_bufferer[32 * 1024];
            const int num_converted_bytes = SDL_AudioStreamGet(stream, convert_bufferer, new_bytes);
            
            //Converted bytes is more than zero
            if (num_converted_bytes > 0) {
                int num_samples = num_converted_bytes / sizeof(float);
                float* samples = reinterpret_cast<float*>(convert_bufferer);
                
                //Add to waveform data
                {
                    std::lock_guard<std::mutex> lock(waveform_mutex);
                    size_t old_size = waveform_data.size();
                    waveform_data.resize(old_size + num_samples); // Resize once
                    std::copy(samples, samples + num_samples, waveform_data.begin() + old_size); // Copy samples
                }

                // Change the volume of the audio
                if (volume_slider_value != 1.0f) {
                    for (int i = 0; i < num_samples; i++)
                        samples[i] *= volume_slider_value;
                }
            }
            SDL_QueueAudio(audio_device, convert_bufferer, new_bytes);
        }
    }
}