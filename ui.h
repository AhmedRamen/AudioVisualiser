#pragma once
#include "button.h"
#include "PlaylistButton.h"

class UI {
public:
	void Render(SDL_Surface* surface) {
		//MyButton.Render(surface);
		button.Render(surface);
	}

	void HandleEvent(const SDL_Event& e) {
		//MyButton.HandleEvent(e);
		button.HandleEvent(e);
	}

	//Button MyButton{ 50,50,50,50 };
	DerivedButton button{ 160, 50, 100, 50 };
};