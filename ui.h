#pragma once
#include "button.h"
#include "PlaylistButton.h"

class UI {
public:

	//Draw everything to the screen with one class
	void Render(SDL_Renderer* renderer) {
		button.Render(renderer);
		bar.Render(renderer);
	}

	//Handle everything with one class (this goes in the while(running) {})
	void HandleEvent(const SDL_Event& e) {
		button.HandleEvent(e);
	}

	//Button MyButton{x position, y position, width, height};
	DerivedButton button{ 160, 50, 100, 50 };

	//GRAY BAR...
	Rectangle bar{ 0, 250, 500, 100, {0, 0, 0, 150}};
};