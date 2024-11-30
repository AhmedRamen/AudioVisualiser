#pragma once

#include "../UIComponents/texturebutton.h"
#include <iostream>

namespace AV {
	class FileButton : public TextureButton {
	public:
		FileButton(int x, int y, int w, int h, std::string spriteSheetPath) : TextureButton{ x,y,w,h, "assets/Buttons.png", 320 } {}

	protected:
		void HandleLeftClick() override {
			setFrame(2);
			std::cout << "test" << std::endl;
		}
	};
}
