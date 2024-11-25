#pragma once
#include "../Components/texture.h"
#include <SDL_ttf.h>
#include "../Components/screen.h"

class TextTexture : public LTexture {
public:
	//Constructor
    TextTexture(const Rectangle& rect, std::string text, SDL_Color color = { 255,255,255,255 }) : mRectangle(rect), text{ loadFromRenderedText(text, color, font, renderer) }, color{ color } {}

	//Load text as texture
	bool loadFromRenderedText(const std::string textureText, SDL_Color textColor, TTF_Font* font, SDL_Renderer* renderer);
    
private:
    //Declare everything again lol
    //The actual hardware texture
    SDL_Texture* mTexture;

    //Compose rectangle to texture
    Rectangle mRectangle;

    //Image dimensions
    int mWidth;
    int mHeight;

    //These are new variables
    std::string text;
    TTF_Font* font = TTF_OpenFont("assets/m5x7.ttf", 32);
    SDL_Color color;
};

inline bool TextTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font, SDL_Renderer* renderer)
{
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
    if (textSurface == NULL)
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    else
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (mTexture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;

            // Update the rectangle dimensions to match the texture size
            mRectangle = Rectangle(mRectangle.getX(), mRectangle.getY(), mWidth, mHeight);
        }

        //Get rid of old surface
        SDL_FreeSurface(textSurface);
    }

    //Return success
    return mTexture != NULL;
}

