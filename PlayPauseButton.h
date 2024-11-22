#pragma once
#include "texturebutton.h"
#include <iostream>
#include "audio.h"

class PlayPauseButton : public TextureButton {
public:
	PlayPauseButton(int x, int y, int w, int h, std::string spriteSheetPath) : TextureButton{x,y,w,h, "assets/Buttons.png"} {}

protected:
	void HandleLeftClick() override {
		std::cout << "Playing audio: " << paused << std::endl;
		setFrame(2); //Clicked
		PauseMusic();
	}
};