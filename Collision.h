#pragma once
#include <SDL.h>

class ColliderComponent;

class Collision {
public:
    static bool AABB(const SDL_Rect& recA, const SDL_Rect& recB);

    static bool AABB(const ColliderComponent& colA, const ColliderComponent& colB);

    static bool AABB(const SDL_Rect& recA, const ColliderComponent& colB);
    static bool AABB(const ColliderComponent& colA, const SDL_Rect& recB);
};

#ifndef COLLISION_H
#define COLLISION_H
#endif //COLLISION_H