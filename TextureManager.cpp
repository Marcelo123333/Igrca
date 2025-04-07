#include "TextureManager.h"


SDL_Texture *TextureManager::LoadTexture(const char *texture) {

    SDL_Surface* tempSurface = IMG_Load(texture);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    return tex;
}

void TextureManager::Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest) {
    // Adjust for the camera offset
    dest.x -= Game::camera.x;
    dest.y -= Game::camera.y;
    SDL_RenderCopy(Game::renderer, tex, &src, &dest);
}