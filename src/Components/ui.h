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
	TextTexture text{ { 100, 100, 200, 50 } };
};

/* Examples
* TextTexture myTextTexture(100, 100, 200, 50); // Position (100, 100) with width 200 and height 50
* 
* // Load text as a texture.
* SDL_Color textColor = {255, 255, 255}; // White color
* 
* if (myTextTexture.loadFromRenderedText("Hello World!", textColor, gFont, renderer))
*    myTextTexture.render(renderer);
*  else
*     printf("Failed to load rendered text!\n");
*/