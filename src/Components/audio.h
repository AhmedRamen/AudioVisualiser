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

    bool repeating = false;

    int* num_samplePointer = NULL;
    float* samplePointer = NULL;

    inline void PauseMusic() {
        paused = paused ? false : true;
        SDL_PauseAudioDevice(audio_device, paused);
    }

    //Destroy the audio and it's properties
    inline void stop_audio() {

        //Don't touch call back as we're freeing the stream.
        SDL_LockAudioDevice(audio_device);
        //Destroy stream
        if (stream) {
            SDL_FreeAudioStream(stream);
        }
        //stream = NULL;
        SDL_AtomicSetPtr((void**)&stream, NULL);
        SDL_UnlockAudioDevice(audio_device);

        //Destroy the current wav buffer
        if (wavbuf) {
            SDL_FreeWAV(wavbuf);
        }


        wavbuf = NULL;
        wavlen = 0;

        //Clear waveform data after destroying audio (or audio error happened)
        std::lock_guard<std::mutex> lock(waveform_mutex);
        waveform_data.clear();
    }

    //Open a file from the specified path
    inline bool OpenAudioFile(const char* fname, SDL_Window* window) {

        SDL_AudioStream* tmpstr = stream;
        //Don't touch call back as we're freeing the stream.
        SDL_LockAudioDevice(audio_device);
        //stream = NULL;
        SDL_AtomicSetPtr((void**)&stream, NULL);
        SDL_UnlockAudioDevice(audio_device);

        SDL_FreeAudioStream(tmpstr);
        //Load audio
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
        tmpstr = SDL_NewAudioStream(wavspec.format, wavspec.channels, wavspec.freq, AUDIO_F32, 2, 48000);
        if (!tmpstr) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Couldn't create audio stream!", SDL_GetError(), window);
            stop_audio();
            return false;
        }

        //Stream didn't work
        if (SDL_AudioStreamPut(tmpstr, wavbuf, wavlen) == -1) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Audio stream put failed!", SDL_GetError(), window);
            stop_audio();
            return false;
        }

        if (SDL_AudioStreamFlush(tmpstr) == -1) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Audio stream flushing failed!", SDL_GetError(), window);
            stop_audio();
            return false;
        }; //Error handling should be fixed.


        //Don't touch call back as we're freeing the stream.
        SDL_LockAudioDevice(audio_device);
        //stream = tmpstr;
        SDL_AtomicSetPtr((void**)&stream, tmpstr);
        SDL_UnlockAudioDevice(audio_device);

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
    inline void SDLCALL __cdecl feed_audio_device_callback(void *userdata, Uint8* output_stream, int len) {
        //Gets more pro
        SDL_AudioStream*input_stream = (SDL_AudioStream *) SDL_AtomicGetPtr((void**) &stream);
        
        //Fill with silence if no stream exists
        if (input_stream == NULL) {
            std::memset(output_stream, 0, len);
            return;
        }

        const int num_converted_bytes = SDL_AudioStreamGet(input_stream, output_stream, len);

        if (num_converted_bytes > 0) {
            const int num_samples = (num_converted_bytes / sizeof(float));
            float* samples = reinterpret_cast<float*>(output_stream);

            /*TODO: rewrite waveform
            // Add samples to waveform data
            {
                std::lock_guard<std::mutex> lock(waveform_mutex);   
                size_t old_size = waveform_data.size();    
                waveform_data.resize(old_size + num_samples); 
                std::copy(samples, samples + num_samples, waveform_data.begin() + old_size);
            }
            */

            // Apply volume adjustment
            for (int i = 0; i < num_samples; ++i)
                samples[i] *= volume_slider_value;
            }

        len -= num_converted_bytes; //Has number of bytes left after feeding device
        output_stream += num_converted_bytes;

        //fill with silence
        if (len > 0)
            std::memset(output_stream, 0, len);
            //SDL_memset(output_stream, '\0', len);
        }
    }
