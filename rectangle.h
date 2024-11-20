#pragma once
#include <SDL.h>

class Rectangle {
public:
	Rectangle(int x, int y, int w, int h, SDL_Color color = { 0,0,0,255 }) : rect{ x,y,w,h}, Color {color} {}

	virtual void Render(SDL_Surface* surface) {
		SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, Color.r, Color.g, Color.b));
	}

	void SetColor(SDL_Color c) { Color = c; }

	//In boundaries from the mouse
    bool IsWithinBounds(int x, int y) const {
        // Too far left
        if (x < rect.x) return false;
        // Too far right
        if (x > rect.x + rect.w) return false;
        // Too high
        if (y < rect.y) return false;
        // Too low
        if (y > rect.y + rect.h) return false;
        // Within bounds
        return true;
    }

private:
	SDL_Rect rect{ 0,0,0,0 };
	SDL_Color Color{ 0,0,0,0 };
};