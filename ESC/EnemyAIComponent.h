#pragma once

#include "Components.h"
#include "TransformComponent.h"
#include "ColliderComponent.h"
#include "../Vector2D.h"
#include <SDL.h>
#include <vector>


class Entity;
extern Entity* player;

class EnemyAIComponent : public Component {
public:

    EnemyAIComponent(float range = 320.0f,
                     float followSpd = 0.3f,
                     float wanderSpd = 0.4f,
                     float radius = 200.0f,
                     Uint32 interval = 2000);

    void init() override;
    void update() override;

private:
    // CHASE
    float sightRange;
    float followSpeed;

    // WANDER
    float wanderSpeed;
    float wanderRadius;
    Uint32 lastWanderChange;
    Uint32 wanderInterval;
    Vector2D wanderDirection;

    // CENTER SPAWNA
    Vector2D spawnCenter;

    // LOS
    bool lineIntersectsRect(const Vector2D& p1,
                            const Vector2D& p2,
                            const SDL_Rect& rect);

    // WANDER Z WALL COLLISIONOM
    void wander(TransformComponent& tx);
};
