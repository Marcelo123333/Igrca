#pragma once

#include "Components.h"
#include "TransformComponent.h"
#include "ColliderComponent.h"
#include "../Vector2D.h"
#include <SDL.h>
#include <vector>

// Forward declaration of Entity
class Entity;
extern Entity* player;

class EnemyAIComponent : public Component {
public:
    // Constructor parameters:
    //  range: vision distance (pixels)
    //  followSpd: speed when chasing the player
    //  wanderSpd: speed when wandering randomly
    //  radius: max wander distance from spawn point
    //  interval: ms between wander direction changes
    EnemyAIComponent(float range = 320.0f,
                     float followSpd = 0.3f,
                     float wanderSpd = 0.4f,
                     float radius = 200.0f,
                     Uint32 interval = 2000);

    void init() override;
    void update() override;

private:
    // Chase behavior
    float sightRange;
    float followSpeed;

    // Wander behavior
    float wanderSpeed;
    float wanderRadius;
    Uint32 lastWanderChange;
    Uint32 wanderInterval;
    Vector2D wanderDirection;

    // Spawn origin for clamping wander
    Vector2D spawnCenter;

    // Helper: line-of-sight vs a rectangular collider
    bool lineIntersectsRect(const Vector2D& p1,
                            const Vector2D& p2,
                            const SDL_Rect& rect);

    // Wander logic with wall collision detection
    void wander(TransformComponent& tx);
};
