#pragma once
#include "../Game.h"
#include "ECS.h"
#include "Components.h"

class TransformComponent;

class KeyboardController : public Component {
public:
    TransformComponent* transform;

    bool boostActive = false;
    Uint32 boostStartTime = 0;
    Uint32 lastBoostTime = 0;

    const Uint32 boostDuration = 300;      // 300ms
    const Uint32 boostDelay = 3000;          // 3000ms

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
    }

    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    void update() override {
        // VSAK FRAME RESETAS VELOCITY
        transform->velocity.x = 0;
        transform->velocity.y = 0;

        if (keystates[SDL_SCANCODE_W])
            transform->velocity.y = -1;
        if (keystates[SDL_SCANCODE_A])
            transform->velocity.x = -1;
        if (keystates[SDL_SCANCODE_S])
            transform->velocity.y = 1;
        if (keystates[SDL_SCANCODE_D])
            transform->velocity.x = 1;

        Uint32 currentTime = SDL_GetTicks();

        // BOOSTAS
        if (keystates[SDL_SCANCODE_C] && !boostActive &&
            (currentTime - lastBoostTime >= boostDelay)) {
            boostActive = true;
            boostStartTime = currentTime;
            transform->speed = 10; // POVECA PLAYER SPEED
        }


        if (boostActive && (currentTime - boostStartTime >= boostDuration)) {
            boostActive = false;
            lastBoostTime = currentTime;
            transform->speed = 5; // NAZAJ NA NORMALNO HITROST
        }
    }
};