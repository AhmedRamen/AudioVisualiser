#pragma once

//Properties
#include "UIComponents/button.h"
#include "Components/texture.h"
#include "UIComponents/texturebutton.h"
#include "UIComponents/textTexture.h"


//Buttons

#include "MainUI/FileButton.h"
//#include "PlaylistButton.h"
#include "MainUI/PlayPauseButton.h"
#include "MainUI/VolumeSlider.h"
#include "../MainUI/StopButton.h"

namespace AV {
	class UI {
	public:
		//Draw everything to the screen with one class
		void Render(SDL_Renderer* renderer) {
			//button.Render(renderer);
			if (!wavbuf)
				text.render(renderer);
			idk.render(renderer, 0, SDL_FLIP_NONE, { 200,100,200,200 });
			bar.Render(renderer);
			play.Render(renderer);
			stop.Render(renderer);
			volumeBar.render(renderer);
			volumeSlider.Render(renderer);
			

		}

		//Handle everything with one class (this goes in the while(running) {})
		void HandleEvent(const SDL_Event& e) {
			//button.HandleEvent(e);
			play.HandleEvent(e);
			stop.HandleEvent(e);
			volumeSlider.HandleEvent(e);
		}

		//Button MyButton{x position, y position, width, height};
		// button{ 160, 50, 100, 50 };
		//Buttons
		PlayPauseButton play{ 400, 335, 64, 64, "./assets/Buttons.png", 0 };
		StopButton stop{ 300, 335, 64, 64, "./assets/Buttons.png", 320 };
		VolumeSlider volumeSlider{ 465,333,125,65, "./assets/VolSlider2.png" };


		//TextureButton play{ 400, 335, 64, 64, "./assets/Buttons.png" };

		//GRAY BAR...
		Rectangle bar{ 0, 330, 600, 70, {50, 50, 50, 150} };

		//Non-interactable Textures
		LTexture idk{ "assets/Star.png", {0, 100, 200, 200} };
		LTexture volumeBar{ "assets/VolSlider1.png", {465, 333, 0, 0} };

		TextTexture text{ { 130, 150, 200, 50 }, "Drag a .wav here to get started", {0, 255, 255, 255} };
	};
}

