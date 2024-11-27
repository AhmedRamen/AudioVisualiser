#pragma once

//Properties
#include "UIComponents/button.h"
#include "Components/texture.h"
#include "UIComponents/textTexture.h"

//Buttons
#include "UIComponents/texturebutton.h"
//#include "PlaylistButton.h"
#include "MainUI/PlayPauseButton.h"

class UI {
public:

	//Draw everything to the screen with one class
	void Render(SDL_Renderer* renderer) {
		//button.Render(renderer);
		idk.render(renderer, 0, SDL_FLIP_NONE, { 200,100,200,200 });
		bar.Render(renderer);
		play.Render(renderer);
		text.render(renderer);
		
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
	LTexture idk{ "assets/Star.png", {100, 100, 200, 200} };
	TextTexture text{ { 200, 100, 200, 50 }, "Hello world", {0, 255, 255, 255} };
};