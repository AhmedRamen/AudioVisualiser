#pragma once
#include <iostream>
#include "../UIComponents/texturebutton.h"
#include "../Components/audio.h"

namespace AV {
	class PlayPauseButton : public TextureButton {
	public:
		PlayPauseButton(int x, int y, int w, int h, std::string spriteSheetPath, int offsetX) : TextureButton{ x,y,w,h, "assets/Buttons.png", 0 } {}

	protected:
		void HandleLeftClick() override {
			std::cout << "Playing audio: " << paused << std::endl;
			if (stream) {
				//Check paused or not
				setYOffset(paused ? 64 : 0);

				PauseMusic();
			}

			//Alert user that action happened
			setFrame(2); //Clicked
		}
	};
}