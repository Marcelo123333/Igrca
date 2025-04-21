#pragma once
#include <SDL.h>
#include <vector>
// forward declarations:
class ColliderComponent;
class Map;

// In Game.h

// In Game.h
class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();

    void handleCollisions(class Vector2D oldPlayerPos);
    bool running() { return isRunning; }

    static bool inMenu;
    static SDL_Texture* menuTexture;
    static SDL_Texture* pauseTexture;
    static SDL_Texture* winTexture;

    static SDL_Event event;
    static SDL_Renderer* renderer;
    static std::vector<ColliderComponent*> colliders;
    static SDL_Rect camera;

    int petCount;
    int storedPets;
    int heartCount;
    Uint32 lastHitTime;

    // ✅ Add these:
    void saveGame();
    void loadGame();

private:
    Uint32 lastMousePositionLogTime = 0;
    const Uint32 mouseLogInterval = 1000;
    int mouseX = 0;
    int mouseY = 0;
    bool showMousePosition = true;
    bool isRunning;
    bool isPaused = false;
    bool gameWon = false;

    SDL_Window* window;
    Map* map;

    int playerStartX = 200;
    int playerStartY = 200;

    void initializeGame();
};


#ifndef GAME_H
#define GAME_H
// ... rest of header
#endif // GAME_H


