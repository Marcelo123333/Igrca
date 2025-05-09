#pragma once
#include "ECS.h"
#include "TransformComponent.h"
#include "SDL.h"
#include "SpriteComponent.h"
class TransformComponent;
class TileComponent : public Component {
public:
    TransformComponent *transform;
    SpriteComponent * sprite;

    SDL_Rect tileRect;
    int tileID;
    const char* path;
    TileComponent() = default;

    TileComponent(int x, int y, int w, int h, int id) {
        tileRect.x = x;
        tileRect.y = y;
        tileRect.w = w;
        tileRect.h = h;
        tileID = id;

        switch(tileID) {
            case 0:
                path = "Assets/Tla.png";
            break;
            case 1:
                path = "Assets/Zid.png";
            break;
            case 2:
                path = "Assets/Trava.png";
            break;
            case 3:
                path = "Assets/Wood.png";
            break;
            case 4:
                path = "Assets/Shelter.png";
            break;
        }
    }

    void init() override {
        entity->addComponent<TransformComponent>((float)tileRect.x, (float)tileRect.y, tileRect.w, tileRect.h, 1);
        transform = &entity->getComponent<TransformComponent>();

        entity->addComponent<SpriteComponent>(path);
        sprite = &entity->getComponent<SpriteComponent>();
    }
};




#ifndef TILECOMPONENT_H
#define TILECOMPONENT_H

#endif //TILECOMPONENT_H