#pragma once
#include <SDL.h>
#include "rectangle.h"

class Button : public Rectangle {
	public:
		Button(int x, int y, int w, int h) : Rectangle{ x,y,w,h, {0,255,0} } {}
	
		//Handle an event
		void HandleEvent(const SDL_Event& e) {
			if (isDisabled) return;
			if (e.type == SDL_MOUSEMOTION)
				HandleMouseMotion(e.motion);
			else if (e.type == SDL_MOUSEBUTTONDOWN)
				HandleMouseButton(e.button);
		};

	protected:
		void SetIsDisabled(bool newValue) {
			isDisabled = newValue;
		}

		virtual void HandleMouseEnter() {
			SetColor({ 255, 0, 0 });
		}
		virtual void HandleMouseExit() {
			SetColor({ 255, 0, 0 });
		}
		virtual void HandleLeftClick(){}
		virtual void HandleRightClick(){}

	private:
		//Handle mouse movement
		void HandleMouseMotion(const SDL_MouseMotionEvent& e) {
			if (IsWithinBounds(e.x, e.y)) {
				HandleMouseEnter();
			}
			else {
				HandleMouseExit();
			}
		}

		//Handle mouse click
		void HandleMouseButton(const SDL_MouseButtonEvent& e) {
			if (IsWithinBounds(e.x, e.y)) {
				const Uint8 Button{ e.button };
				if (Button == SDL_BUTTON_LEFT)
					HandleLeftClick();
				else if (Button == SDL_BUTTON_RIGHT)
					HandleRightClick();
			}
		}

		bool isDisabled{ false };
};