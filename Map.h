#include "Game.h"

class Map {
public:
    Map();
    ~Map();

    void LoadMap(int arr[20][25]);
    void DrawMap();



private:

    SDL_Rect src, dest;
    SDL_Texture* tla;
    SDL_Texture* zid;


    int map[20][25];

};





#ifndef MAP_H
#define MAP_H

#endif //MAP_H