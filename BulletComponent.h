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

        // Save old position
        float oldX = transform.position.x;
        float oldY = transform.position.y;

        // Move the bullet using its normalized direction and speed
        transform.position.x += direction.x * speed;
        transform.position.y += direction.y * speed;

        // Explicitly update the bullet's collider so it matches the new transform.
        auto& bulletCollider = entity->getComponent<ColliderComponent>();
        bulletCollider.update();

        // Calculate the distance traveled this frame and accumulate it
        float dx = transform.position.x - oldX;
        float dy = transform.position.y - oldY;
        distanceTraveled += std::sqrt(dx * dx + dy * dy);

        // Check collision with wall colliders (those with tag "wall")
        for (auto& col : Game::colliders) {
            if (col->tag == "wall") {
                if (Collision::AABB(bulletCollider, *col)) {
                    entity->destroy();  // Destroy the bullet upon collision
                    return;             // Exit update early
                }
            }
        }

        // If the bullet has traveled beyond its maximum allowed distance, destroy it
        if (distanceTraveled >= maxDistance) {
            entity->destroy();
        }
    }

};
