using namespace std;
#pragma once
#include <SDL.h>
#include <iostream>
#include <stdio.h>
#include <SDL_image.h>
#include <vector>
#include "Vector2D.h"
#ifndef GAME_H
#define GAME_H

class ColliderComponent;

class Game {
public:
    Game();
    ~Game();
    void init(const char* title, int width, int height, bool fullscreen);

    void handleEvents();
    void update();
    void render();
    void clean();

    // New method for collision handling
    void handleCollisions(Vector2D oldPlayerPos);

    bool running() { return isRunning; }
    static SDL_Event event;
    static SDL_Renderer* renderer;
    static vector<ColliderComponent*> colliders;

private:
    int cnt = 0;
    bool isRunning;
    SDL_Window *window;
};

#endif //GAME_H