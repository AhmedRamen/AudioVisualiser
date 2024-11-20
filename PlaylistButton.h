#pragma once
#include "button.h"
#include <iostream>
#include "audio.h"

class DerivedButton : public Button {
public:
	DerivedButton(int x, int y, int w, int h) : Button{x,y,w,h}{}

protected:
	void HandleLeftClick() override {
		std::cout << "Playing audio: " << paused << std::endl;
		PauseMusic();
	}

	void HandleRightClick() override {
		std::cout << "I have been right-clicked\n";
	}
};