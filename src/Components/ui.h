#pragma once
#include <iostream>

//Properties
#include "UIComponents/button.h"
#include "Components/texture.h"
#include "UIComponents/texturebutton.h"
#include "UIComponents/textTexture.h"


//Buttons

#include "MainUI/RepeatButton.h"
//#include "PlaylistButton.h"
#include "MainUI/PlayPauseButton.h"
#include "MainUI/VolumeSlider.h"
#include "../MainUI/StopButton.h"
#include "../MainUI/SeekBar.h"

namespace AV {
	class UI {
	public:
		//Draw everything to the screen with one class
		void Render(SDL_Renderer* renderer) {
			//button.Render(renderer);
			if (!current_sample)
				text.render(renderer);

			idk.render(renderer, 0, SDL_FLIP_NONE, { 200,100,200,200 });
			bar.Render(renderer);
			topbar.Render(renderer);
			play.Render(renderer);
			stop.Render(renderer);
			//SeekBar.render(renderer);
			volumeBar.render(renderer);
			volumeSlider.Render(renderer);
			repeat.Render(renderer);

			if (paused)
				pauseText.render(renderer);

			//Seeker.Render(renderer);

		}

		//Handle everything with one class (this goes in the while(running) {})
		void HandleEvent(const SDL_Event& e) {
			//button.HandleEvent(e);
			play.HandleEvent(e);
			stop.HandleEvent(e);
			repeat.HandleEvent(e);
			volumeSlider.HandleEvent(e);
			//Seeker.HandleEvent(e);
		}

		//Button MyButton{x position, y position, width, height};
		// button{ 160, 50, 100, 50 };
		//Buttons
		PlayPauseButton play{ 400, 335, 64, 64, "./assets/Buttons.png", 0 };
		StopButton stop{ 330, 335, 64, 64, "./assets/Buttons.png", 320 };
		RepeatButton repeat{ 260, 335, 64, 64, "./assets/Buttons.png" };
		VolumeSlider volumeSlider{ 465,333,125,65, "./assets/VolSlider2.png" };
		
		//SeekBar Seeker{ 30, 333, 0, 0 , "./assets/ProgressBar2.png",  };

		//TextureButton play{ 400, 335, 64, 64, "./assets/Buttons.png" };

		//GRAY BAR...
		Rectangle bar{ 0, 330, 600, 70, {0, 0, 0, 150} };
		Rectangle topbar{ 0, -20, 600, 70, {0, 0, 0, 150} };

		//Non-interactable Textures
		LTexture idk{ "assets/Star.png", {0, 100, 200, 200} };
		LTexture volumeBar{ "assets/VolSlider1.png", {465, 333, 0, 0} };
		//LTexture SeekBar{ "assets/ProgressBar1.png", {30, 333, 0, 0 } };

		TextTexture text{ { 90, 200, 200, 50 }, "Drag an audio file here to get started", {0, 255, 255, 255} };
		TextTexture pauseText{ {50,335,0,0},"Currently paused", {255,255,255,255}};
	};
}

