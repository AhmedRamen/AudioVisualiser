#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "rectangle.h"
#include "../Components/screen.h"


namespace AV {
	class TextureButton : public Rectangle {
	public:
		TextureButton(int x, int y, int w, int h, const std::string& spriteSheetPath, int offsetY = 0) : Rectangle{ x,y,w,h, {0,255,0} }, spriteSheet(LoadTexture(spriteSheetPath.c_str(), renderer)), offsetY(0) {
			buttonFrames[0] = { 0,offsetY,w,h }; //Normal
			buttonFrames[1] = { w,offsetY,w,h }; //Hovering
			buttonFrames[2] = { w * 2,offsetY,w,h }; //Clicked

		}

		//Handle an event
		void HandleEvent(const SDL_Event& e) {
			if (isDisabled) return;

			if (e.type == SDL_MOUSEMOTION)
				HandleMouseMotion(e.motion);
			else if (e.type == SDL_MOUSEBUTTONDOWN)
				HandleMouseButton(e.button);
			else if (e.type == SDL_MOUSEBUTTONUP)
				HandleMouseButtonUp(e.button);
		};


		//Fix Render
		void Render(SDL_Renderer* renderer) {
			SDL_Rect destRect = { getX(), getY(), getWidth(), getHeight() };
			SDL_RenderCopy(renderer, spriteSheet, &buttonFrames[currentFrame], &destRect);
		}

		void setFrame(int newFrame) { currentFrame = newFrame; }

		void setYOffset(int newOffsetY) { offsetY = newOffsetY; }

	protected:
		void SetIsDisabled(bool newValue) {
			isDisabled = newValue;
		}

		//Hovering at the button area
		virtual void HandleMouseEnter() {
			currentFrame = 1; //Hover sprite
		}

		//Exiting the button area
		virtual void HandleMouseExit() {
			currentFrame = 0; //Normal sprite
		}

		//Clicking on the 
		virtual void HandleLeftClick() {
			currentFrame = 2; //Clicked sprite
		}

		virtual void HandleRightClick() {}

		SDL_Texture* LoadTexture(const std::string path, SDL_Renderer* renderer) {
			SDL_Surface* surface = IMG_Load(path.c_str());
			//No image actually exists
			if (!surface) {
				fprintf(stderr, "File %s couldn't be found!", path.c_str());
				return nullptr;
			}

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
			return texture;
		}


	private:

		//Handle mouse movement
		virtual void HandleMouseMotion(const SDL_MouseMotionEvent& e) {
			if (IsWithinBounds(e.x, e.y)) {
				HandleMouseEnter();
			}
			else {
				HandleMouseExit();
			}
		}

		//Handle mouse click
		virtual void HandleMouseButton(const SDL_MouseButtonEvent& e) {
			if (IsWithinBounds(e.x, e.y)) {
				const Uint8 Button{ e.button };
				if (Button == SDL_BUTTON_LEFT) {
					HandleLeftClick();
				}
				else if (Button == SDL_BUTTON_RIGHT)
					HandleRightClick();
			}
		}

		virtual void HandleMouseButtonUp(const SDL_MouseButtonEvent& e) {
			if (IsWithinBounds(e.x, e.y)) {
				currentFrame = 1; // Return to hover state on release
			}
		}



		bool isDisabled{ false };

		SDL_Texture* spriteSheet;
		SDL_Rect buttonFrames[3];
		int currentFrame;
		int offsetY;
	};
}
