#pragma once
#include "../Game.h"
#include "ECS.h"
#include "Components.h"

class TransformComponent;

class KeyboardController : public Component {
public:
    TransformComponent* transform;

    // Boost state variables:
    bool boostActive = false;
    // The moment the boost started (in milliseconds)
    Uint32 boostStartTime = 0;
    // The moment the last boost ended or was triggered, used to enforce a cooldown
    Uint32 lastBoostTime = 0;
    // Boost parameters in milliseconds:
    const Uint32 boostDuration = 500;      // Boost lasts 0.5 sec (500 ms)
    const Uint32 boostDelay = 3000;          // Cooldown period of 5 sec (5000 ms)

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
    }

    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    void update() override {
        // Reset velocity each frame.
        transform->velocity.x = 0;
        transform->velocity.y = 0;

        // Basic movement keys.
        if (keystates[SDL_SCANCODE_UP])
            transform->velocity.y = -1;
        if (keystates[SDL_SCANCODE_LEFT])
            transform->velocity.x = -1;
        if (keystates[SDL_SCANCODE_DOWN])
            transform->velocity.y = 1;
        if (keystates[SDL_SCANCODE_RIGHT])
            transform->velocity.x = 1;

        // Get the current time.
        Uint32 currentTime = SDL_GetTicks();

        // If D is pressed, boost is not active, and enough time has passed since last boost,
        // trigger the boost.
        if (keystates[SDL_SCANCODE_D] && !boostActive &&
            (currentTime - lastBoostTime >= boostDelay)) {
            boostActive = true;
            boostStartTime = currentTime;
            transform->speed = 10; // Increase player's speed.
        }

        // If boost is active, check if the boost duration has elapsed.
        if (boostActive && (currentTime - boostStartTime >= boostDuration)) {
            boostActive = false;
            lastBoostTime = currentTime; // Record when boost ended.
            transform->speed = 5; // Revert player speed back to normal.
        }
    }
};