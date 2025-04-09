#pragma once
#include <SDL.h>
#include <vector>
// forward declarations:
class ColliderComponent;
class Map;

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

    static SDL_Event event;
    static SDL_Renderer* renderer;
    static std::vector<ColliderComponent*> colliders;
    static SDL_Rect camera;

    // Pet counter: how many pets the player has picked up.
    int petCount;

private:
    int cnt = 0;
    bool isRunning;
    SDL_Window* window;
    Map* map;
};
