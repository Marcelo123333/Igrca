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
    bool isInMenu() const { return inMenu; }

    static SDL_Event event;
    static SDL_Renderer* renderer;
    static std::vector<ColliderComponent*> colliders;
    static SDL_Rect camera;

    int storedPets = 0;
    int petCount;
    int heartCount;
    Uint32 lastHitTime;

private:
    Uint32 lastMousePositionLogTime = 0;
    const Uint32 mouseLogInterval = 1000;
    int mouseX = 0;
    int mouseY = 0;
    bool showMousePosition = true;
    int cnt = 0;
    bool isRunning;
    SDL_Window* window;
    Map* map;

    // Player starting position defaults.
    int playerStartX = 200;  // Default starting position X
    int playerStartY = 200;  // Default starting position Y

    // initializeGame still takes no parameters.
    void initializeGame();
};

#ifndef GAME_H
#define GAME_H
// ... rest of header
#endif // GAME_H


