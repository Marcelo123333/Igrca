#pragma once
#include "Game.h"

class TextureManager {

public:
    static SDL_Texture* LoadTexture(const char* fileName);
    static void Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest);
};

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

// Your class definition here

#endif // TEXTUREMANAGER_H
