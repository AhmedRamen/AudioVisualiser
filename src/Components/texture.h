#pragma once
#include <SDL.h>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include "../UIComponents/rectangle.h"

//Taken entirely from lazyfoo

//Texture wrapper class
class LTexture
{
public:
    
    LTexture() : mTexture(NULL), mRectangle{ 0, 0, 0, 0 }, mWidth(0), mHeight(0) {}

    // Default constructor
    LTexture(const std::string& texturePath, Rectangle rect) : mTexture(NULL), mRectangle(rect) {
        if (!loadFromFile(texturePath, renderer))
            fprintf(stderr, "Failed to draw texture: %s\n", SDL_GetError());
    }

    //Deallocates memory
    ~LTexture();

    //Loads image at specified path
    bool loadFromFile(std::string path, SDL_Renderer* renderer);

    //Deallocates texture
    void free();

    //Set color modulation
    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    //Set blending
    void setBlendMode(SDL_BlendMode blending);

    //Set alpha modulation
    void setAlpha(Uint8 alpha);

    //Renders texture at given point
    void render(SDL_Renderer* renderer,
        SDL_Point* center = NULL,
        SDL_RendererFlip flip = SDL_FLIP_NONE,
        const Rectangle& rectangle = {0, 0, 0, 0});

    //Gets image properties
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    void setRotation(double angle);
    void setPosition(int x, int y);

private:
    //The actual hardware texture
    SDL_Texture* mTexture;
    
    //Compose rectangle to texture
    Rectangle mRectangle;

    //Image dimensions
    int mWidth;
    int mHeight;
    double mAngle = 0.0;

};

//Deallocate texture
LTexture::~LTexture() { free(); }

//Load from file
inline bool LTexture::loadFromFile(std::string path, SDL_Renderer* renderer) {
    //Get rid of preexisting texture
    free();


    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    else
    {
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (mTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
            SDL_FreeSurface(loadedSurface);
        }
        else
        {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;

            mRectangle = Rectangle(mRectangle.getX(), mRectangle.getY(), mWidth, mHeight);

            //Get rid of old loaded surface
            SDL_FreeSurface(loadedSurface);
        }
    }

    //Return success
    return true;
}

//Free texture
inline void LTexture::free() {
    //Free texture if it exists
    if (mTexture != NULL) {
        SDL_DestroyTexture(mTexture); //FIXME: crashes when you close for some reason
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;

        //Reset rectangle dimensions if needed
        mRectangle = Rectangle(mRectangle.getX(), mRectangle.getY(), 0, 0);
    }
}

//Modulate texture
inline void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) { SDL_SetTextureColorMod(mTexture, red, green, blue); }

//Set blending function
inline void LTexture::setBlendMode(SDL_BlendMode blending) { SDL_SetTextureBlendMode(mTexture, blending); }

//Modulate texture alpha
inline void LTexture::setAlpha(Uint8 alpha) { SDL_SetTextureAlphaMod(mTexture, alpha); }

//Once again, pass renderer to here because we don't know any because of screen.h
inline void LTexture::render(SDL_Renderer* renderer, SDL_Point* center, SDL_RendererFlip flip, const Rectangle& rectangle)
{
    //Get the SDL_Rect from Rectangle class
    SDL_Rect renderQuad = { getX(), getY(), getWidth(), getHeight()};



    //Render to screen
    if (mTexture != NULL)
        SDL_RenderCopyEx(renderer, mTexture, NULL, &renderQuad, mAngle, center ? center : NULL, flip);
}

// Getters for rectangle properties
inline int LTexture::getX() const { return mRectangle.getX(); }

inline int LTexture::getY() const { return mRectangle.getY(); }

inline int LTexture::getWidth() const { return mRectangle.getWidth(); }

inline int LTexture::getHeight() const { return mRectangle.getHeight(); }

inline void LTexture::setRotation(double angle) { mAngle = angle; }

inline void LTexture::setPosition(int x, int y) {
    //too lazy to overload operator =
    mRectangle.setX(x);
    mRectangle.setY(y);
    mRectangle = {x, y,getWidth(),getHeight() };
}
