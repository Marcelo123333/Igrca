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

    // Load the map from the text file instead of a hardcoded array
    LoadMap();

    // Initialize source and destination rectangles for tile drawing
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
    std::ifstream mapFile("Mape/CelaMapa.txt"); // Adjust the file path if needed
    if (!mapFile.is_open()) {
        std::cout << "Unable to open map file!" << std::endl;
        return;
    }

    std::string line;
    int row = 0;
    while (std::getline(mapFile, line) && row < 230) {
        int col = 0;
        // Loop through each character in the line
        for (char c : line) {
            // Check if the character is a digit
            if (isdigit(c) && col < 230) {
                // Convert character to integer (assuming single-digit numbers)
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
    // Draw the map based on the values in the map array
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
                default:
                    break;
            }
        }
    }
}

void Map::CreateWallColliders(Manager& manager) {
    // Iterate through the map array to create colliders for wall tiles (value 1)
    for (int row = 0; row < 230; row++) {
        for (int column = 0; column < 230; column++) {
            if (map[row][column] == 1) { // If it's a wall
                auto& wall(manager.addEntity());

                // Add transform and collider components
                wall.addComponent<TransformComponent>(
                    static_cast<float>(column * 32),
                    static_cast<float>(row * 32),
                    32, 32, 1
                );
                wall.addComponent<ColliderComponent>("wall");

                // Initialize the components immediately
                wall.getComponent<TransformComponent>().update();
                wall.getComponent<ColliderComponent>().update();
            }
        }
    }
}
