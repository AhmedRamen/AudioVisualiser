#pragma once
#include <SDL.h>

class Rectangle {
public:
	Rectangle(int x, int y, int w, int h, SDL_Color color = { 0,0,0, 255 }) : rect{ x,y,w,h}, Color {color} {}

	virtual void Render(SDL_Renderer* renderer) {
        //Draw colour
        SDL_SetRenderDrawColor(renderer, Color.r, Color.g, Color.b, Color.a);
        
        //Draw rectangle
        SDL_RenderFillRect(renderer, &rect);
	}

	void SetColor(SDL_Color c) { Color = c; }

	//In boundaries from the mouse
    bool IsWithinBounds(int x, int y) const {
        return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
    }

private:
	SDL_Rect rect{ 0,0,0,0 };
	SDL_Color Color{ 0,0,0,0 };
};