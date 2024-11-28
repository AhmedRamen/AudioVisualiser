#pragma once
#include <SDL.h>
#include <stdio.h>



Uint64 now = SDL_GetPerformanceCounter();
Uint64 last = 0;
double deltaTime;

//This would save all the trouble if this was seperate.
SDL_Window* window;
SDL_Renderer* renderer;

//Panics and stops the program from executing any further
inline void panic_and_abort(const char* title, const char* text) {
	fprintf(stderr, "PANIC: %s ... %s\n", title, text);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, NULL); //should point to window but NULL will work.
	SDL_Quit();
	exit(1);
}

class Window {
	public:
		Window() { 
			window = SDL_CreateWindow("AudioVisualee", 
				SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
				screen_width, screen_height, 0); 
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
			//couldn't create renderer
			if (!renderer) {
				panic_and_abort("Renderer couldn't be created!", SDL_GetError());
			}
		}
		
		//Render the screen
		void Render() { SDL_RenderClear(renderer); }

		//Update screen
		void Update() { 
			SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
			SDL_RenderPresent(renderer); }

		//Get surface
		SDL_Renderer* GetRenderer() { return SDL_GetRenderer(window); }

		//Audio waveform shenanigans
		int getWidth() { return screen_width; }
		int getHeight() { return screen_height; }


		//Clean up
		~Window() {
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
		}

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		SDL_Window* getWindow() { return window; }

	private:
		int screen_width = 600;
		int screen_height = 400;
};