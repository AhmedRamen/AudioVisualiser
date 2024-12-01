#pragma once
#include "../Components/texture.h"
#include <SDL_ttf.h>
#include "../Components/screen.h"

namespace AV {
    TTF_Font* font; //Okay just make this global

    class TextTexture : public LTexture {
    public:
        //Constructor
        TextTexture(const Rectangle& rect, std::string text, SDL_Color color = { 255,255,255,255 }) : mRectangle(rect), text(text), color{ color }, mTexture(NULL), mWidth(0), mHeight(0) {
            //Initialize SDL_ttf
            if (TTF_Init() == -1)
                printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

            font = TTF_OpenFont("assets/main.ttf", 32);
            //Font doesn't exist
            if (font == NULL)
                fprintf(stderr, "Failed to load font. SDL_ERROR: %s\n", TTF_GetError());

            //Renderer doesn't exist...???
            if (renderer) {
                if (!loadFromRenderedText(text, color, renderer)) {
                    fprintf(stderr, "Failed to draw text: %s\n", SDL_GetError());
                };
            }
            else {
                panic_and_abort("Renderer doesn't exist!", SDL_GetError());
            }
        }

        //Load text as texture
        bool loadFromRenderedText(const std::string textureText, SDL_Color textColor, SDL_Renderer* renderer);

        void free();

        ~TextTexture() {
            free();
            if (font) {
                TTF_CloseFont(font); //fixme: this crashes on closing (which shouldn't matter but it's annoying when coding!!!!)
                font = NULL;
            }
        }

        void render(SDL_Renderer*) {
            SDL_Rect renderQuad = { mRectangle.getX(), mRectangle.getY(), mWidth, mHeight};

            SDL_RenderCopy(renderer, mTexture, NULL, &renderQuad);
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
                printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
            else
            {
                //Get image dimensions
                mWidth = textSurface->w;
                mHeight = textSurface->h;
            }

            //Get rid of old surface
            SDL_FreeSurface(textSurface);
        }

        //Return success
        return mTexture != NULL;
    }
}