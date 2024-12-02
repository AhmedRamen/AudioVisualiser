#pragma once
#include <iostream>
#include "../UIComponents/texturebutton.h"
#include "../Components/audio.h"

namespace AV {
	class PlayPauseButton : public TextureButton {
	public:
		PlayPauseButton(int x, int y, int w, int h, std::string spriteSheetPath, int offsetY) : TextureButton{ x,y,w,h, "assets/Buttons.png"} {}

	protected:

		void HandleLeftClick() override {
			if (current_sample) {
				//Check paused or not
				PauseMusic();
			}

			//Alert user that action happened
			setFrame(2); //Clicked
			if (paused) {
				setYOffset(64);
			}
			else {
				setYOffset(128);
			}
		}
	};
}