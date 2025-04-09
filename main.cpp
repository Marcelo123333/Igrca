#include <windows.h>
#include <cstddef>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Game.h"

int main(int argc, char* argv[]) {
    // Optionally allocate a console (if you need one)
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);

    Game* game = new Game();
    game->init("Igrica", 800, 640, false);

    const int FPS = 60;
    const int frameDelay = 1000 / FPS; // milliseconds per frame

    Uint32 frameStart;
    int frameTime;

    // Main game loop
    while (game->running()) {
        frameStart = SDL_GetTicks();
        game->handleEvents();
        game->update();
        game->render();
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime)
            SDL_Delay(frameDelay - frameTime);
    }

    game->clean();
    delete game;
    return 0;
}
