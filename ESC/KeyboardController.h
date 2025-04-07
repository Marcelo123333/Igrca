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
        
        transform->velocity.x = 0;
        transform->velocity.y = 0;


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

        // Boundary checking
        int screenWidth = 800;
        int screenHeight = 640;
        int margin = 32;


        float nextX = transform->position.x + transform->velocity.x * transform->speed;
        float nextY = transform->position.y + transform->velocity.y * transform->speed;


        if (nextX < margin) {
            nextX = margin;
            transform->velocity.x = 0;
        }
        if (nextY < margin) {
            nextY = margin;
            transform->velocity.y = 0;
        }
        if (nextX + transform->width > screenWidth - margin) {
            nextX = screenWidth - margin - transform->width;
            transform->velocity.x = 0; // Stop horizontal movement
        }
        if (nextY + transform->height > screenHeight - margin) {
            nextY = screenHeight - margin - transform->height;
            transform->velocity.y = 0; // Stop vertical movement
        }

        // Update the player's position
        transform->position.x = nextX;
        transform->position.y = nextY;
    }
};

#ifndef KEYBOARDCONTROLLER_H
#define KEYBOARDCONTROLLER_H

#endif // KEYBOARDCONTROLLER_H#