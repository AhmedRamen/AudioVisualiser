#pragma once
#include "button.h"

class DerivedButton : public Button {
public:
	DerivedButton(int x, int y, int w, int h) : Button{x,y,w,h} {}
protected:
	void HandleLeftClick() override {

	}
};