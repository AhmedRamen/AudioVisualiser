#pragma once
#include <SDL.h>
#include <iostream>
#include <mutex>
#include <vector>
#include <algorithm>
#include "screen.h"




namespace AV {

#define audio_sample_rate 48000
#define audio_channels 2
#define audio_buffer_length (audio_sample_rate / 32)

    static SDL_AudioDeviceID audio_device = 0;

    std::mutex waveform_mutex;

    std::vector<float> waveform_data; //Store waveform samples

    SDL_AudioSpec desired;

    bool paused = true;

    //Volume
    float volume_slider_value = 1.0f;

    float audio_buffer[audio_buffer_length * audio_channels];

    Uint8* wavbuf = NULL;
    Uint32 wavlen = 0;
    SDL_AudioSpec wavspec;
    SDL_AudioStream* stream = NULL;


    int* num_samplePointer = NULL;
    float* samplePointer = NULL;

    inline void PauseMusic() {
        paused = paused ? false : true;
        SDL_PauseAudioDevice(audio_device, paused);
    }

    //Destroy the audio and it's properties
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

        //Clear waveform data after destroying audio (or audio error happened)
        std::lock_guard<std::mutex> lock(waveform_mutex);
        waveform_data.clear();
    }

    //Open a file from the specified path
    inline bool OpenAudioFile(const char* fname, SDL_Window* window) {
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
            return false;
        }

        //Create stream (if we can that is)
        stream = SDL_NewAudioStream(wavspec.format, wavspec.channels, wavspec.freq, AUDIO_F32, 2, 48000);
        if (!stream) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Couldn't create audio stream!", SDL_GetError(), window);
            stop_audio();
            return false;
        }

        //Stream didn't work
        if (SDL_AudioStreamPut(stream, wavbuf, wavlen) == -1) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Audio stream put failed!", SDL_GetError(), window);
            stop_audio();
            return false;
        }

        if (SDL_AudioStreamFlush(stream) == -1) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Audio stream flushing failed!", SDL_GetError(), window);
            stop_audio();
            return false;
        }; //Error handling should be fixed.

        return true;

        // Copy data into audio buffer for visualization
        std::memcpy(audio_buffer, wavbuf, std::min(wavlen, static_cast<Uint32>(sizeof(audio_buffer))));

        return true;
    }

    //Draw the waveform based on the WAV
// Function to draw waveform
    inline void drawWaveform(SDL_Renderer* renderer, float* samples, int num_samples) {
        float spp = static_cast<float>(screen_width) / audio_buffer_length; // Samples per pixel
        float halfY = screen_height / 2.0f;
        float quarterY = halfY / 2.0f;

        std::lock_guard<std::mutex> lock(waveform_mutex);

        for (int i = 1; i < num_samples && i < audio_buffer_length; i++) {
            int x1 = static_cast<int>((i - 1) * spp);
            int x2 = static_cast<int>(i * spp);

            // Draw left channel in green
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderDrawLine(renderer,
                x1,
                halfY - (quarterY * samples[(i - 1) * 2]),
                x2,
                halfY - (quarterY * samples[i * 2]));

            // Draw right channel in yellow
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderDrawLine(renderer,
                x1,
                halfY - (quarterY * samples[((i - 1) * 2) + 1]),
                x2,
                halfY - (quarterY * samples[(i * 2) + 1]));
        }

        SDL_RenderPresent(renderer); // Present the rendered frame
    }


    //Update stream
    inline void AudioStreamUpdate() {
        const int min_audio_buffer_size = 8192;
        const int max_new_bytes = 32 * 1024; // Maximum bytes to queue

        if (SDL_GetQueuedAudioSize(audio_device) < min_audio_buffer_size) {
            const int bytes_remaining = SDL_AudioStreamAvailable(stream);
            if (bytes_remaining > 0) {
                const Uint32 new_bytes = SDL_min(bytes_remaining, static_cast<Uint32>(max_new_bytes));
                Uint8 convert_bufferer[max_new_bytes];

                const int num_converted_bytes = SDL_AudioStreamGet(stream, convert_bufferer, new_bytes);
                if (num_converted_bytes > 0) {
                    int num_samples = num_converted_bytes / sizeof(float);
                    float* samples = reinterpret_cast<float*>(convert_bufferer);

                    // Apply volume adjustment
                    for (int i = 0; i < num_samples; ++i) {
                        samples[i] *= volume_slider_value;
                    }

                    // Add samples to waveform data
                    {
                        std::lock_guard<std::mutex> lock(waveform_mutex);
                        size_t old_size = waveform_data.size();
                        waveform_data.resize(old_size + num_samples);
                        std::copy(samples, samples + num_samples, waveform_data.begin() + old_size);
                    }

                    // Draw only when new samples are added
                    num_samplePointer = &num_samples;
                    samplePointer = samples;
                    SDL_QueueAudio(audio_device, convert_bufferer, num_converted_bytes);
                }
            }
        }
    }    
}
