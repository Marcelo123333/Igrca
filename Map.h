#pragma once
#include "Game.h"
#include "ESC/ECS.h"

class Map {
public:
    Map();
    ~Map();

    void LoadMap();
    void DrawMap();

    // New method to create wall colliders
    void CreateWallColliders(Manager& manager);

private:
    SDL_Rect src, dest;
    SDL_Texture* tla;
    SDL_Texture* zid;

    int map[100][100];
};

#ifndef MAP_H
#define MAP_H

#endif //MAP_H