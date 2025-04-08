#pragma once
#include "ESC/Components.h"
#include "ESC/TransformComponent.h"
#include "Vector2D.h"
#include <cmath>

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
        // Ensure the entity has a transform component
        if (!entity->hasComponent<TransformComponent>()) {
            entity->addComponent<TransformComponent>();
        }
    }

    void update() override {
        auto& transform = entity->getComponent<TransformComponent>();

        // Save the old position
        float oldX = transform.position.x;
        float oldY = transform.position.y;

        // Move the bullet based on its direction and speed
        transform.position.x += direction.x * speed;
        transform.position.y += direction.y * speed;

        // Calculate distance traveled this frame and accumulate it
        float dx = transform.position.x - oldX;
        float dy = transform.position.y - oldY;
        distanceTraveled += std::sqrt(dx * dx + dy * dy);

        // Destroy the bullet entity when it exceeds maxDistance
        if (distanceTraveled >= maxDistance) {
            entity->destroy();
        }
    }
};
