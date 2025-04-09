#pragma once
#include "ESC/Components.h"
#include "ESC/TransformComponent.h"
#include "Vector2D.h"
#include "ESC/ColliderComponent.h"
#include "Collision.h"
#include <cmath>
#include <iostream>

class BulletComponent : public Component {
public:
    Vector2D direction;    // Normalized direction vector
    float speed;           // Bullet speed (pixels per frame)
    float distanceTraveled;
    float maxDistance;     // Maximum distance (in pixels) the bullet travels

    BulletComponent(Vector2D dir, float spd, float maxDist)
        : direction(dir), speed(spd), distanceTraveled(0), maxDistance(maxDist)
    {}

    void init() override {
        // Ensure the bullet has a TransformComponent and a ColliderComponent
        if (!entity->hasComponent<TransformComponent>()) {
            entity->addComponent<TransformComponent>();
        }
        if (!entity->hasComponent<ColliderComponent>()) {
            // The tag can be "bullet"; it's not used for collision resolution in this case,
            // since we're only interested in detecting collision against walls.
            entity->addComponent<ColliderComponent>("bullet");
        }
    }

    void update() override {
        auto& transform = entity->getComponent<TransformComponent>();

        // Save old position for distance calculation.
        float oldX = transform.position.x;
        float oldY = transform.position.y;

        // Move the bullet along its normalized direction at the defined speed.
        transform.position.x += direction.x * speed;
        transform.position.y += direction.y * speed;

        // Update distance traveled.
        float dx = transform.position.x - oldX;
        float dy = transform.position.y - oldY;
        distanceTraveled += std::sqrt(dx * dx + dy * dy);

        // Update the bullet's collider after moving.
        auto& bulletCollider = entity->getComponent<ColliderComponent>();
        bulletCollider.update();

        // First, check collision against wall colliders.
        for (auto& col : Game::colliders) {
            if (col->tag == "wall") {
                if (Collision::AABB(bulletCollider, *col)) {
                    // Destroy the bullet if it hits a wall.
                    entity->destroy();
                    return;
                }
            }
        }

        // Next, check collision against enemy colliders.
        for (auto& col : Game::colliders) {
            if (col->tag == "enemy") {
                if (Collision::AABB(bulletCollider, *col)) {
                    // If the bullet collides with an enemy, destroy both enemy and bullet.
                    col->entity->destroy();  // Destroy enemy.
                    entity->destroy();         // Destroy bullet.
                    return;
                }
            }
        }

        // If the bullet has traveled beyond its maximum allowed distance, destroy it.
        if (distanceTraveled >= maxDistance) {
            entity->destroy();
        }
    }

};
