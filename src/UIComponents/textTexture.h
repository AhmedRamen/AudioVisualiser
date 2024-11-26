#pragma once
#include "../Components/texture.h"
#include <SDL_ttf.h>
#include "../Components/screen.h"

class TextTexture : public LTexture {
public:
	//Constructor
    TextTexture(const Rectangle& rect, std::string text, SDL_Color color = { 255,255,255,255 }) : mRectangle(rect), text(text), color{color} {
    font = TTF_OpenFont("assets/m5x7.ttf", 16);
    //Font doesn't exist
    if (!font)
        fprintf(stderr, "Failed to load font. SDL_ERROR: %s\n", TTF_GetError());

    //Renderer doesn't exist...???
    if (renderer && font)
        loadFromRenderedText(text, color, renderer);
    }

	//Load text as texture
	bool loadFromRenderedText(const std::string textureText, SDL_Color textColor, SDL_Renderer* renderer);

    void free();

    ~TextTexture() {
        free();
        if (font) {
            TTF_CloseFont(font);
            font = NULL;
        }
    }
    
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
    TTF_Font* font;
    SDL_Color color;


};

// Free texture if it exists 
inline void TextTexture::free() {
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
        mWidth = 0;
        mHeight = 0;
    }
}

inline bool TextTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor, SDL_Renderer* renderer)
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

