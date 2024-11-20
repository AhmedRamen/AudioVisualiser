#pragma once
#include <SDL.h>
#include <stdio.h>

int screen_width = 500;
int screen_height = 300;

Uint64 now = SDL_GetPerformanceCounter();
Uint64 last = 0;
double deltaTime;


class Window {
	public:
		Window() { window = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, 0); }
		
		//Render the screen
		void Render() {
			SDL_FillRect(GetSurface(), nullptr, SDL_MapRGB(GetSurface()->format, 50, 50, 50));
		}

		//Update screen
		void Update() {
			SDL_UpdateWindowSurface(window);
		}

		//Get surface
		SDL_Surface* GetSurface() {
			return SDL_GetWindowSurface(window);
		}

		
		//Clean up
		~Window() {
			SDL_DestroyWindow(window);
		}

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		SDL_Window* getWindow() {
			return window;
		}

	private:
		SDL_Window* window;
};


//Panics and stops the program from executing any further
inline void panic_and_abort(const char* title, const char* text) {
	fprintf(stderr, "PANIC: %s ... %s\n", title, text);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, NULL); //should point to window but hopefully NULL will work.
	SDL_Quit();
	exit(1);
}