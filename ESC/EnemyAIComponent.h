#pragma once
#include "Components.h"
#include "TransformComponent.h"
#include "ColliderComponent.h"
#include "../Vector2D.h"
#include "../Collision.h"
#include <cmath>
#include <iostream>

// Ensure that the global player variable (pointer) is declared somewhere
extern Entity* player;

class EnemyAIComponent : public Component {
public:
    // Sight range in pixels; for 10 tiles of 32 pixels each, use 320.
    float sightRange;
    // Speed at which the enemy follows the player.
    float followSpeed;

    EnemyAIComponent(float range = 320.0f, float speed = 0.1f)
        : sightRange(range), followSpeed(speed)
    {}

    // Helper function: given two points, check if the line between them intersects the given SDL_Rect.
    // This simple routine checks intersection with each edge of the rect.
    bool lineIntersectsRect(const Vector2D& p1, const Vector2D& p2, const SDL_Rect& rect) {
        auto lineIntersects = [&](const Vector2D& a, const Vector2D& b,
                                    const Vector2D& c, const Vector2D& d) -> bool {
            float denominator = (b.x - a.x) * (d.y - c.y) - (b.y - a.y) * (d.x - c.x);
            if (denominator == 0) return false; // parallel lines
            float ua = ((d.x - c.x) * (a.y - c.y) - (d.y - c.y) * (a.x - c.x)) / denominator;
            float ub = ((b.x - a.x) * (a.y - c.y) - (b.y - a.y) * (a.x - c.x)) / denominator;
            return (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1);
        };

        // Define rectangle corners.
        Vector2D topLeft(rect.x, rect.y);
        Vector2D topRight(rect.x + rect.w, rect.y);
        Vector2D bottomLeft(rect.x, rect.y + rect.h);
        Vector2D bottomRight(rect.x + rect.w, rect.y + rect.h);
        // Check each edge of the rectangle.
        if (lineIntersects(p1, p2, topLeft, topRight)) return true;
        if (lineIntersects(p1, p2, topRight, bottomRight)) return true;
        if (lineIntersects(p1, p2, bottomRight, bottomLeft)) return true;
        if (lineIntersects(p1, p2, bottomLeft, topLeft)) return true;

        return false;
    }

    void update() override {
        // Get enemy transform.
        auto& enemyTransform = entity->getComponent<TransformComponent>();
        // Compute enemy center point.
        Vector2D enemyCenter(
            enemyTransform.position.x + enemyTransform.width * enemyTransform.scale / 2,
            enemyTransform.position.y + enemyTransform.height * enemyTransform.scale / 2
        );

        // Check that player exists.
        if (!player) return;
        // Get player's transform and center.
        auto& playerTransform = player->getComponent<TransformComponent>();
        Vector2D playerCenter(
            playerTransform.position.x + playerTransform.width * playerTransform.scale / 2,
            playerTransform.position.y + playerTransform.height * playerTransform.scale / 2
        );

        // Compute the distance between enemy and player.
        float dx = playerCenter.x - enemyCenter.x;
        float dy = playerCenter.y - enemyCenter.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        // If the player is out of sight range, stop following.
        if (distance > sightRange) {
            enemyTransform.velocity.x = 0;
            enemyTransform.velocity.y = 0;
            return;
        }

        // Otherwise, check for any wall between enemy and player.
        bool blocked = false;
        // Assume wall colliders are stored in Game::colliders and tagged with "wall".
        for (auto& col : Game::colliders) {
            if (col->tag == "wall") {
                if (lineIntersectsRect(enemyCenter, playerCenter, col->collider)) {
                    blocked = true;
                    break;
                }
            }
        }

        if (blocked) {
            // If blocked, enemy stops.
            enemyTransform.velocity.x = 0;
            enemyTransform.velocity.y = 0;
        } else {
            // Clear line of sight: enemy sets velocity toward the player.
            enemyTransform.velocity.x = (dx / distance) * followSpeed;
            enemyTransform.velocity.y = (dy / distance) * followSpeed;
        }
    }
};
