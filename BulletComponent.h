#pragma once

#include "ESC/Components.h"
#include "ESC/TransformComponent.h"
#include "ESC/ColliderComponent.h"
#include "Vector2D.h"
#include "Collision.h"
#include "Game.h"           // for Game::colliders
#include <SDL.h>            // for SDL_GetTicks()
#include <cmath>
#include <iostream>

class BulletComponent : public Component {
public:
    Vector2D direction;    // Normalized direction vector
    float    speed;        // pixels per frame
    float    distanceTraveled = 0.0f;
    float    maxDistance;  // in pixels

    BulletComponent(const Vector2D& dir, float spd, float maxDist)
      : direction(dir), speed(spd), maxDistance(maxDist)
    {}

    void init() override {
        // enforce 1 s cooldown between bullets
        static Uint32 lastShotTime = 0;
        Uint32 now = SDL_GetTicks();
        if (now - lastShotTime < 700) {
            // too soon — cancel this bullet
            entity->destroy();
            return;
        }
        lastShotTime = now;

        // ensure we have a transform & collider
        if (!entity->hasComponent<TransformComponent>()) {
            entity->addComponent<TransformComponent>();
        }
        if (!entity->hasComponent<ColliderComponent>()) {
            entity->addComponent<ColliderComponent>("bullet");
        }
    }

    void update() override {
        auto& tx = entity->getComponent<TransformComponent>();

        // remember old pos
        float oldX = tx.position.x;
        float oldY = tx.position.y;

        // move
        tx.position.x += direction.x * speed;
        tx.position.y += direction.y * speed;

        // accumulate distance
        float dx = tx.position.x - oldX;
        float dy = tx.position.y - oldY;
        distanceTraveled += std::sqrt(dx*dx + dy*dy);

        // update collider
        auto& bc = entity->getComponent<ColliderComponent>();
        bc.update();

        // collide with walls
        for (auto& colPtr : Game::colliders) {
            if (colPtr->tag == "wall" && Collision::AABB(bc, *colPtr)) {
                entity->destroy();
                return;
            }
        }

        // collide with enemies
        for (auto& colPtr : Game::colliders) {
            if (colPtr->tag == "enemy"
             && colPtr->entity->isActive()
             && Collision::AABB(bc, *colPtr))
            {
                colPtr->entity->destroy();
                entity->destroy();
                return;
            }
        }

        // timeout
        if (distanceTraveled >= maxDistance) {
            entity->destroy();
        }
    }
};
