#pragma once
#include <SDL.h>
#include "../SDL_Sound/SDL_Sound.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>

namespace AV {

    static SDL_AudioDeviceID audio_device = 0;

    Sound_AudioInfo audio_device_spec;

    static Sound_Sample* current_sample = NULL;

    Uint32 sample_available = 0;
    Uint32 sample_position = 0;

    
    int processed_samples = 0;


    // Circular buffer for waveform data
    const size_t max_waveform_size = 1024; // Maximum number of samples to visualize
    std::mutex waveform_data_mutex;
    std::vector<float> waveform_data(max_waveform_size, 0.0f);
    size_t write_index = 0; // Index for writing new samples

    SDL_AudioSpec desired;

    bool paused = true;

    //Volume
    float volume_slider_value = 1.0f;

    bool repeating = false;

    inline void PauseMusic() {
        paused = paused ? false : true;
        SDL_PauseAudioDevice(audio_device, paused);
    }

    //Destroy the audio and it's properties
    inline void stop_audio() {
        Sound_Sample* sample = current_sample;

        //Don't touch call back as we're freeing the stream.
        if (sample) {
            SDL_LockAudioDevice(audio_device);
            //Destroy sample
            //stream = NULL;
            SDL_AtomicSetPtr((void**)&current_sample, NULL);
            SDL_UnlockAudioDevice(audio_device);
            Sound_FreeSample(sample);
        }

        sample_available = 0;
        sample_position = 0;

        //Clear waveform data after destroying audio (or audio error happened)
        waveform_data.clear();
    }

    //Open a file from the specified path
    inline bool OpenAudioFile(const char* fname, SDL_Window* window) {

        stop_audio();

        //Function above should've set it to NULL.
        SDL_assert(current_sample == NULL);

        //Create new samples from new file
        Sound_Sample* sample = Sound_NewSampleFromFile(fname, &audio_device_spec, 64 * 1024);

        if (!sample) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Audio file cannot be loaded!", Sound_GetError(), window);
            return false;
        }

        //Allow audio callback to read sample
        SDL_LockAudioDevice(audio_device);
        //sample = current_sample;
        SDL_AtomicSetPtr((void**)&current_sample, sample);
        SDL_UnlockAudioDevice(audio_device);

        // Copy data into audio buffer for visualization
        //std::memcpy(audio_buffer, wavbuf, std::min(wavlen, static_cast<Uint32>(sizeof(audio_buffer))));
        
        return true;
    }

    // Function to draw waveform based on how loud the song is
    inline void drawWaveform(SDL_Renderer* renderer, int width, int height) {
        
        //Nothing to draw, return nothing
        if (waveform_data.empty()) return;

        SDL_RenderClear(renderer);

        //Lock mutex to access the data
        std::lock_guard<std::mutex> lock(waveform_data_mutex);

        // Set drawing color for waveform
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 155); // Green color for waveform

        // Calculate scaling factors
        float max_amplitude = *std::max_element(waveform_data.begin(), waveform_data.end());
        float min_amplitude = *std::min_element(waveform_data.begin(), waveform_data.end());

        //Avoid division by zero
        float amplitude_range = max_amplitude - min_amplitude;
        if (amplitude_range == 0) amplitude_range = 1;

        float x_scale = static_cast<float>(width) / max_waveform_size; // Scale x-axis
        float y_scale = (height / 2 ) / amplitude_range; // Scale y-axis

        //Center waveform
        float waveform_center = (max_amplitude + min_amplitude) / 2;

        // Draw waveform
        for (size_t i = 1; i < max_waveform_size; ++i) {
            size_t index1 = (write_index + i - 1) % max_waveform_size; // Previous index in circular buffer
            size_t index2 = (write_index + i) % max_waveform_size;     // Current index in circular buffer

            //Get line points
            int x1 = static_cast<int>((i - 1) * x_scale);
            int y1 = SDL_clamp(static_cast<int>(height / 2 - (waveform_data[i - 1] - waveform_center) * y_scale), 0, height);
            int x2 = static_cast<int>(i * x_scale);
            int y2 = SDL_clamp(static_cast<int>(height / 2 - (waveform_data[i] - waveform_center) * y_scale), 0, height);

            //Draw line
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
        SDL_RenderPresent(renderer);
    }

    //Update stream
    inline void SDLCALL __cdecl feed_audio_device_callback(void* userdata, Uint8* output_stream, int len) {
        //Gets more pro
        Sound_Sample* sample = (Sound_Sample*)SDL_AtomicGetPtr((void**)&current_sample);

        //Fill with silence if no stream exists
        if (sample == NULL) {
            std::memset(output_stream, 0, len);
            return;
        }

        while (len > 0) {
            if (sample_available == 0) {
                const Uint32 br = Sound_Decode(sample);
                //End of the current song.
                if (br == 0 && !repeating) {
                    Sound_FreeSample(current_sample);
                    SDL_AtomicSetPtr((void**)&current_sample, NULL);
                    sample_available = 0;
                    sample_position = 0;
                    //Write silence and clear waveform data after destroying audio
                    std::memset(output_stream, 0, len);
                    waveform_data.clear();
                    write_index = 0;
                    return;
                }
                else if (br == 0 && repeating) {
                    Sound_Rewind(sample);
                }
                sample_available = br;
                sample_position = 0;
            }

            float* samples = reinterpret_cast<float*>(output_stream);
            const Uint32 cpy = SDL_min(sample_available, (Uint32)len);
            SDL_assert(cpy > 0);
            SDL_memcpy(samples, static_cast<const Uint8*>(sample->buffer) + sample_position, cpy);

            const int num_samples = (int)(cpy / sizeof(float));

            // Apply volume adjustment & waveform visualisation
            float max_value = *SDL_max(samples, samples + num_samples);

            std::lock_guard<std::mutex> lock(waveform_data_mutex);

            for (int i = 0; i < num_samples; ++i) {
                samples[i] *= volume_slider_value;

                // Store samples for waveform visualization
                if (waveform_data.size() < max_waveform_size) {
                    waveform_data.push_back(samples[i]);
                }
                else {
                    // Shift left to make room for new samples
                    waveform_data.erase(waveform_data.begin());
                    waveform_data.push_back(samples[i]);
                }
            }

            //Update processeed samples
            processed_samples += cpy / sizeof(float);

            //Chunk audio
            len -= cpy;
            output_stream += cpy;
            sample_available -= cpy;
            sample_position += cpy;
        }
    }
}
