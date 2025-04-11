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
    static bool inMenu;
    static SDL_Texture* menuTexture;
    bool isInMenu() const { return inMenu; }

    static SDL_Event event;
    static SDL_Renderer* renderer;
    static std::vector<ColliderComponent*> colliders;
    static SDL_Rect camera;

    int petCount;      // Already used elsewhere.
    int heartCount;    // Number of lives (hearts) for the player.
    Uint32 lastHitTime; // Timestamp (in ms) of the last enemy collision.


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
    void initializeGame();
};
