#pragma once
#include <string>
#include "SDL.h"
#include "Components.h"

class ColliderComponent : public Component {
public:
    SDL_Rect collider;
    string tag;

    TransformComponent* transform;

    ColliderComponent(string t) {
        tag = t;
    }
    void init() override {
        if(!entity->hasComponent<TransformComponent>()) {
            entity->addComponent<TransformComponent>();
        }
        transform = &entity->getComponent<TransformComponent>();

        Game::colliders.push_back(this);
    }

    void update() override {
        // POGLEDAS CE IMAMO SPLOH TRANSFORM COMPONENT
        if (!transform) return;

        // DOBIS POZICIJO IN VELIKOST ZA TRANSFORM COMPONENT
        int baseX = static_cast<int>(transform->position.x);
        int baseY = static_cast<int>(transform->position.y);
        int baseW = transform->width * transform->scale;
        int baseH = transform->height * transform->scale;

        if (tag == "enemy") {
            // ZMANSAS HITBOX GLEDENA SPRITE
            float factor = 0.6f; // 60% NJEGOVE VELIKOSTI

            // IZRAČUNAŠ OFFSET DA LAHKO DOBIS NJEGOV NOV HITBOX
            int offsetX = static_cast<int>(baseW * (1 - factor) / 2);
            int offsetY = static_cast<int>(baseH * (1 - factor) / 2);

            collider.x = baseX + offsetX;
            collider.y = baseY + offsetY;
            collider.w = static_cast<int>(baseW * factor);
            collider.h = static_cast<int>(baseH * factor);
        } else {
            collider.x = baseX;
            collider.y = baseY;
            collider.w = baseW;
            collider.h = baseH;
        }
    }


};





#ifndef COLLIDERCOMPONENT_H
#define COLLIDERCOMPONENT_H

#endif //COLLIDERCOMPONENT_H
#pragma once
