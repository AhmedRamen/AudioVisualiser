#pragma once
#include <SDL.h>

namespace AV {
    class Rectangle {
    public:
        Rectangle(int x, int y, int w, int h, SDL_Color color = { 0,0,0, 255 }) : rect{ x,y,w,h }, Color{ color } {}

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

        //Get x, y, w or h
        int getX() const { return rect.x; }
        int getY() const { return rect.y; }
        int getWidth() const { return rect.w; }
        int getHeight() const { return rect.h; }\

            //Set position
            void setX(int x) { rect.x = x; }
        void setY(int y) { rect.x = y; }

        SDL_Rect getSDLRect() { return rect; }

    private:
        SDL_Rect rect{ 0,0,0,0 };
        SDL_Color Color{ 0,0,0,0 };
    };
}