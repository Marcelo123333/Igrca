#include "Map.h"
#include "TextureManager.h"
#include "ESC/ColliderComponent.h"
#include "ESC/TransformComponent.h"

// Hardcoded level map with walls(1) and floors(0)
int lvl1[20][25] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

Map::Map() {
    zid = TextureManager::LoadTexture("Assets/Zid.png");
    tla = TextureManager::LoadTexture("Assets/Tla.png");

    LoadMap();

    src.x = src.y = 0;
    src.w = dest.w = 32;
    src.h = dest.h = 32;

    dest.x = dest.y = 0;
}

Map::~Map() {
    SDL_DestroyTexture(tla);
    SDL_DestroyTexture(zid);
}

void Map::LoadMap() {
    // Copy the hardcoded level map to our map array
    for(int row = 0; row < 20; row++) {
        for(int column = 0; column < 25; column++) {
            map[row][column] = lvl1[row][column];
        }
    }
}

void Map::DrawMap() {
    int type = 0;

    for(int row = 0; row < 20; row++) {
        for(int column = 0; column < 25; column++) {
            type = map[row][column];

            dest.x = column * 32;
            dest.y = row * 32;

            switch (type) {
                case 0:
                    TextureManager::Draw(tla, src, dest);
                    break;
                case 1:
                    TextureManager::Draw(zid, src, dest);
                    break;
                default:
                    break;
            }
        }
    }
}

void Map::CreateWallColliders(Manager& manager) {
    for(int row = 0; row < 20; row++) {
        for(int column = 0; column < 25; column++) {
            if(map[row][column] == 1) { // If it's a wall
                auto& wall(manager.addEntity());

                // Add transform and collider components
                wall.addComponent<TransformComponent>(
                    static_cast<float>(column * 32),
                    static_cast<float>(row * 32),
                    32, 32, 1
                );
                wall.addComponent<ColliderComponent>("wall");

                // Initialize the components
                wall.getComponent<TransformComponent>().update();
                wall.getComponent<ColliderComponent>().update();
            }
        }
    }
}