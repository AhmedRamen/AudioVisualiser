#pragma once

#include "../UIComponents/texturebutton.h"
#include "../Components/audio.h"
#include <iostream>

namespace AV {
	class RepeatButton : public TextureButton {
	public:
		RepeatButton(int x, int y, int w, int h, std::string spriteSheetPath) : TextureButton{ x,y,w,h, "assets/Buttons.png", 128 } {}

	protected:
		void HandleLeftClick() override {
			repeating = repeating ? true : false;
		}
	};
}
