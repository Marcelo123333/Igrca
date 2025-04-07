#pragma once
#include "../Game.h"
#include "ECS.h"
#include "Components.h"

class KeyboardController : public Component {
public:
    TransformComponent* transform;

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
    }

    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    void update() override {
        // Reset velocity
        transform->velocity.x = 0;
        transform->velocity.y = 0;

        // Set velocity based on key presses
        if (keystates[SDL_SCANCODE_UP]) {
            transform->velocity.y = -1;
        }
        if (keystates[SDL_SCANCODE_LEFT]) {
            transform->velocity.x = -1;
        }
        if (keystates[SDL_SCANCODE_DOWN]) {
            transform->velocity.y = 1;
        }
        if (keystates[SDL_SCANCODE_RIGHT]) {
            transform->velocity.x = 1;
        }

        // No more boundary checking - collision system handles this now
    }
};

#ifndef KEYBOARDCONTROLLER_H
#define KEYBOARDCONTROLLER_H

#endif // KEYBOARDCONTROLLER_H