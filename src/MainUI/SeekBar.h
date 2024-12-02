#pragma once
#include "../UIComponents/texturebutton.h"
#include "../Components/audio.h"


namespace AV {
	class SeekBar : public TextureButton {
	public:
		SeekBar(int x, int y, int w, int h, const std::string& sliderPath, float minValue = 0.0f, float maxValue = 100.0f) : TextureButton(x, y, w, h, sliderPath), sliderTexture(LoadTexture(sliderPath, renderer)), sliderPosition(0), minValue(minValue), maxValue(maxValue), currentValue(minValue), currentPlaybackPos(0) {}

		//Handle an event
		void HandleEvent(const SDL_Event& e) {
			if (e.type == SDL_MOUSEMOTION)
				HandleMouseMotion(e.motion);
			else if (e.type == SDL_MOUSEBUTTONDOWN)
				HandleMouseButton(e.button);
			else if (e.type == SDL_MOUSEBUTTONUP)
				HandleMouseButtonUp(e.button);

			UpdateSliderPosition(e.type);
		};

		//Override render
		void Render(SDL_Renderer* renderer) override {
			//Render button
			TextureButton::Render(renderer);

			//Crop width
			int cropWidth = static_cast<int>((currentValue - minValue) / (maxValue - minValue) * getWidth());
			SDL_Rect cropRect = { 0,0,cropWidth, getHeight() };
			SDL_Rect destRect = { getX(), getY(), cropWidth, getHeight() };

			// Now render the slider
			//SDL_Rect sliderRect = { getX() + sliderPosition, getY(), getWidth() / 10, getHeight() };

			SDL_RenderCopy(renderer, sliderTexture, &cropRect, &destRect);
		}
	protected:

		void HandleMouseEnter() override { return; }

		void HandleMouseExit() override { return; }

		void HandleMouseMotion(const SDL_MouseMotionEvent& e) override {
			if (IsWithinBounds(e.x, e.y)) {
				HandleMouseEnter();
				//Drag the thing
				if (isDragging) {
					UpdateSliderPosition(e.x);
				}
				else
					HandleMouseExit();
			}
		}

		void HandleMouseButton(const SDL_MouseButtonEvent& e) override {
			if (IsWithinBounds(e.x, e.y)) {
				if (e.button == SDL_BUTTON_LEFT) {
					HandleLeftClick();
					isDragging = true;
					UpdateSliderPosition(e.x);
				}
			}
		}

		void HandleMouseButtonUp(const SDL_MouseButtonEvent& e) override {
			if (isDragging)
				isDragging = false;
		}

	private:
		SDL_Texture* sliderTexture;
		int sliderPosition; //Position of slider
		float minValue;
		float maxValue;
		float currentValue;
		float currentPlaybackPos;

		//Dragging or not
		bool isDragging{ false };



		void UpdateSliderPosition(int mouseX) {
			//Calculate new position based on X coordinate & boundaries
			float normalizedX = static_cast<float>(mouseX - getX()) / getWidth();

			//Clamping via 0 to 1
			if (normalizedX < 0.0f) normalizedX = 0.0f;
			if (normalizedX > 1.0f) normalizedX = 1.0f;

			if (isDragging) {
				currentValue = minValue + normalizedX * (maxValue - minValue);

				//Seeking
				Sound_Seek(current_sample, currentValue / maxValue);


			}
			currentValue = processed_samples / (maxValue - minValue);
			printf("Current Value: %f\n", currentValue); // Debug

		}
	};
}
