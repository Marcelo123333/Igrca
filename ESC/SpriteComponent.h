#pragma once

#include "Components.h"
#include "SDL.h"
#include "../TextureManager.h"

class TransformComponent;
class SpriteComponent : public Component {
private:
    TransformComponent *transform;
    SDL_Texture *texture;
    SDL_Rect srcRect, destRect;

public:
    SpriteComponent() = default;
    SpriteComponent(const char* path) {
        texture =  TextureManager::LoadTexture(path);
    }

    ~SpriteComponent() {
        SDL_DestroyTexture(texture);
    }

    void init() override {
        transform = &entity->getComponent<TransformComponent>();

        srcRect.x = srcRect.y = 0;
        srcRect.w = transform->width;
        srcRect.h = transform->height;
    }
    void update() override {
        destRect.x = static_cast<int>(transform->position.x);
        destRect.y = static_cast<int>(transform->position.y);
        destRect.w = transform->width * transform->scale;
        destRect.h = transform->height * transform->scale;
    }
    void draw() override {
        TextureManager::Draw(texture, srcRect, destRect);
    }
};



#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#endif //SPRITECOMPONENT_H