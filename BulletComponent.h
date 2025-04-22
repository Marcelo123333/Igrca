#pragma once

#include "ESC/Components.h"
#include "ESC/TransformComponent.h"
#include "ESC/ColliderComponent.h"
#include "Vector2D.h"
#include "Collision.h"
#include "Game.h"
#include <SDL.h>
#include <cmath>
#include <iostream>

class BulletComponent : public Component {
public:
    Vector2D direction;
    float    speed;
    float    distanceTraveled = 0.0f;
    float    maxDistance;

    BulletComponent(const Vector2D& dir, float spd, float maxDist)
      : direction(dir), speed(spd), maxDistance(maxDist)
    {}

    void init() override {
        static Uint32 lastShotTime = 0;
        Uint32 now = SDL_GetTicks();
        if (now - lastShotTime < 700) {
            // DELAY
            entity->destroy();
            return;
        }
        lastShotTime = now;

        // PREVERIMO CE IMAMO TRANSFORM COLLIDER
        if (!entity->hasComponent<TransformComponent>()) {
            entity->addComponent<TransformComponent>();
        }
        if (!entity->hasComponent<ColliderComponent>()) {
            entity->addComponent<ColliderComponent>("bullet");
        }
    }

    void update() override {
        auto& tx = entity->getComponent<TransformComponent>();

        // STARA POZICIJA
        float oldX = tx.position.x;
        float oldY = tx.position.y;

        // PREMAKNES
        tx.position.x += direction.x * speed;
        tx.position.y += direction.y * speed;

        // DOLZINA
        float dx = tx.position.x - oldX;
        float dy = tx.position.y - oldY;
        distanceTraveled += std::sqrt(dx*dx + dy*dy);

        // UPDATAS COLLIDERJE
        auto& bc = entity->getComponent<ColliderComponent>();
        bc.update();

        // CE SE COLLIDA Z ZIDOM
        for (auto& colPtr : Game::colliders) {
            if (colPtr->tag == "wall" && Collision::AABB(bc, *colPtr)) {
                entity->destroy();
                return;
            }
        }

        // CE SE COLLIDA Z ENEMIJOM
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


        if (distanceTraveled >= maxDistance) {
            entity->destroy();
        }
    }
};
