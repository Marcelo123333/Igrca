#include "ESC/ColliderComponent.h"
#include "ESC/TransformComponent.h"
#include "Map.h"
#include "TextureManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>

Map::Map() {
    tla = TextureManager::LoadTexture("Assets/Tla.png");
    zid = TextureManager::LoadTexture("Assets/Zid.png");
    trava = TextureManager::LoadTexture("Assets/Trava.png");
    wood = TextureManager::LoadTexture("Assets/Wood.png");
    shelter = TextureManager::LoadTexture("Assets/Shelter.png");
    trava2 = TextureManager::LoadTexture("Assets/Trava2.png");


    LoadMap();


    src.x = src.y = 0;
    src.w = dest.w = 32;
    src.h = dest.h = 32;
    dest.x = dest.y = 0;
}

Map::~Map() {
    SDL_DestroyTexture(tla);
    SDL_DestroyTexture(zid);
    SDL_DestroyTexture(wood);
    SDL_DestroyTexture(shelter);
    SDL_DestroyTexture(trava2);
}

void Map::LoadMap() {
    std::ifstream mapFile("Mape/CelaMapa.txt");
    if (!mapFile.is_open()) {
        std::cout << "Unable to open map file!" << std::endl;
        return;
    }

    std::string line;
    int row = 0;
    while (std::getline(mapFile, line) && row < 230) {
        int col = 0;

        for (char c : line) {

            if (isdigit(c) && col < 230) {
                map[row][col] = c - '0';
                col++;
            }
        }
        row++;
    }
    mapFile.close();
}


void Map::DrawMap() {
    int type = 0;
    for (int row = 0; row < 230; row++) {
        for (int column = 0; column < 230; column++) {
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
                case 2:
                    TextureManager::Draw(trava, src, dest);
                break;
                case 3:
                    TextureManager::Draw(wood, src, dest);
                break;
                case 4:
                    TextureManager::Draw(shelter, src, dest);
                break;
                case 5:
                    TextureManager::Draw(trava2, src, dest);
                break;
                default:
                    break;
            }
        }
    }
}

void Map::CreateWallColliders(Manager& manager) {
    for (int row = 0; row < 230; row++) {
        for (int column = 0; column < 230; column++) {
            if (map[row][column] == 1 || map[row][column] == 3)  {  //CE JE ZID DODA COLLIDER
                auto& wall(manager.addEntity());


                wall.addComponent<TransformComponent>(
                    static_cast<float>(column * 32),
                    static_cast<float>(row * 32),
                    32, 32, 1
                );
                wall.addComponent<ColliderComponent>("wall");


                wall.getComponent<TransformComponent>().update();
                wall.getComponent<ColliderComponent>().update();
            }
        }
    }
}
