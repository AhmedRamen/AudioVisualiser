#pragma once
#include "../UIComponents/texturebutton.h"
#include "../Components/audio.h"
#include <iostream>

namespace AV {
	class StopButton : public TextureButton {
	public:
		StopButton(int x, int y, int w, int h, std::string spriteSheetPath, int offsetX) : TextureButton{ x,y,w,h, "assets/Buttons.png", 320 } {}
	protected:
		void HandleLeftClick() override {
			setFrame(2);
			if (stream) {
				std::cout << "Stopping audio..." << std::endl;
				stop_audio();
			}
		}
	};
}