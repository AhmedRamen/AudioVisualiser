#pragma once
#include "button.h"
#include "texturebutton.h"
//#include "PlaylistButton.h"
#include "PlayPauseButton.h"

class UI {
public:

	//Draw everything to the screen with one class
	void Render(SDL_Renderer* renderer) {
		//button.Render(renderer);
		bar.Render(renderer);
		play.Render(renderer);
	}

	//Handle everything with one class (this goes in the while(running) {})
	void HandleEvent(const SDL_Event& e) {
		//button.HandleEvent(e);
		play.HandleEvent(e);
	}

	//Button MyButton{x position, y position, width, height};
	// button{ 160, 50, 100, 50 };
	PlayPauseButton play{ 400, 335, 64, 64, "./assets/Buttons.png" };

	//TextureButton play{ 400, 335, 64, 64, "./assets/Buttons.png" };

	//GRAY BAR...
	Rectangle bar{ 0, 330, 600, 70, {0, 0, 0, 150}};
	
};