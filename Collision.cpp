#pragma once
#include "Collision.h"
#include "ESC/ColliderComponent.h"
class Collision;
bool Collision::AABB(const SDL_Rect& recA, const SDL_Rect& recB) {
    if(
        recA.x + recA.w >= recB.x &&
        recB.x + recB.w >= recA.x &&
        recA.y + recA.h >= recB.y &&
        recB.y + recB.h >= recA.y
    ) {
        return true;
    }
    return false;
}

bool Collision::AABB(const ColliderComponent& colA, const ColliderComponent& colB) {
    if(AABB(colA.collider, colB.collider)) {
        cout << colA.tag << " hit: " << colB.tag << endl;
        return true;
    }
    else {
        return false;
    }
}

bool Collision::AABB(const SDL_Rect& recA, const ColliderComponent& colB) {
    return AABB(recA, colB.collider);
}

bool Collision::AABB(const ColliderComponent& colA, const SDL_Rect& recB) {
    return AABB(colA.collider, recB);
}