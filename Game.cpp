// Game.cpp

// Define Windows macros if needed.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <cstddef>      // For std::byte
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>
#include "Game.h"
#include "ESC/Components.h"
#include "ESC/ColliderComponent.h"
#include "Map.h"
#include "ESC/EnemyAIComponent.h"
#include "TextureManager.h"
#include "BulletComponent.h"   // Include bullet component header
#include "Collision.h"         // Include collision header

// Undefine any conflicting macros.
#ifdef byte
#undef byte
#endif

// Global instances used by Game.cpp:
Manager manager;
SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;
std::vector<ColliderComponent*> Game::colliders;
SDL_Rect Game::camera = { 0, 0, 800, 640 };
// Global player pointer (declared as extern in your EnemyAIComponent.h or Globals.h)
Entity* player = nullptr;
bool Game::inMenu = true;
SDL_Texture* Game::menuTexture = nullptr;
Game::Game() : isRunning(false), window(nullptr), map(nullptr) { }
Game::~Game() { }

void spawnEnemy(Manager& manager, float x, float y) {
    auto& enemy = manager.addEntity();
    // Set enemy transform size (visual/collision dimensions) and assign a slower speed.
    enemy.addComponent<TransformComponent>(x, y, 72, 72, 1); // speed is 1 (instead of 5)
    enemy.addComponent<SpriteComponent>("Assets/Nasprotnik.png");
    enemy.addComponent<ColliderComponent>("enemy");
    // Use a follow speed such as 0.05f or 0.1f; adjust until it feels right.
    enemy.addComponent<EnemyAIComponent>(320.0f, 0.5f);
}

// Helper function to spawn a pet at static coordinates.
void spawnPet(Manager& manager, float x, float y) {
    auto& pet = manager.addEntity();
    // Set the pet's transform: adjust size as needed (here 32x32 pixels)
    pet.addComponent<TransformComponent>(x, y, 48, 48, 1);
    // Add a sprite for the pet (ensure "Assets/Pet.png" exists)
    pet.addComponent<SpriteComponent>("Assets/Zival.png");
    // Add a collider with the tag "pet" so that collisions can be detected
    pet.addComponent<ColliderComponent>("pet");
}
void Game::init(const char* title, int width, int height, bool fullscreen) {
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_RESIZABLE;
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        std::cout << "Subsystems Initialized!!" << std::endl;
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
        renderer = SDL_CreateRenderer(window, -1, 0);
        if (window) {
            std::cout << "Window Created!" << std::endl;
        }
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            SDL_RenderSetLogicalSize(renderer, 800, 640);
            std::cout << "Renderer Created!" << std::endl;
        }
        isRunning = true;
    }
    lastMousePositionLogTime = SDL_GetTicks();
    // Load menu texture
    menuTexture = TextureManager::LoadTexture("Assets/Menu.png");
    if (!menuTexture) {
        std::cout << "Failed to load menu texture!" << std::endl;
    }

    // Initialize game state
    inMenu = true;
}
#include "Game.h"
// ... other includes ...

void Game::initializeGame() {
    // Create your map and wall colliders.
    map = new Map();
    map->LoadMap();
    map->CreateWallColliders(manager);

    // Spawn the player using the stored start coordinates.
    // If no mouse click occurs, these will remain at the default values (200, 200)
    player = &manager.addEntity();
    player->addComponent<TransformComponent>(playerStartX, playerStartY, 40, 43, 1);
    player->addComponent<SpriteComponent>("Assets/Clovek.png");
    player->addComponent<KeyboardController>();
    player->addComponent<ColliderComponent>("player");

    // Spawn enemies.
    spawnEnemy(manager, 5775.0f, 5645.0f);
    spawnEnemy(manager, 6117.0f, 5319.0f);
    spawnEnemy(manager, 5992.0f, 4749.0f);
    spawnEnemy(manager, 6462.0f, 4734.0f);
    spawnEnemy(manager, 5938.0f, 4509.0f);
    spawnEnemy(manager, 6314.0f, 2022.0f);
    spawnEnemy(manager, 6430.0f, 2131.0f);
    spawnEnemy(manager, 5772.0f, 1211.0f);
    spawnEnemy(manager, 5710.0f, 550.0f);
    spawnEnemy(manager, 2435.0f, 2166.0f);
    spawnEnemy(manager, 3191.0f, 2637.0f);
    spawnEnemy(manager, 3578.0f, 3489.0f);
    spawnEnemy(manager, 3854.0f, 4170.0f);
    spawnEnemy(manager, 1413.0f, 4490.0f);
    spawnEnemy(manager, 1630.0f, 4670.0f);
    spawnEnemy(manager, 1473.0f, 3615.0f);
    spawnEnemy(manager, 1600.0f, 3730.0f);

    // Initialize game stats.
    petCount = 0;
    heartCount = 3;
    lastHitTime = 0;


    spawnPet(manager, 6563.0f, 4801.0f);
    spawnPet(manager, 6156.0f, 4520.0f);
    spawnPet(manager, 6119.0f, 1555.0f);
    spawnPet(manager, 5825.0f, 886.0f);
    spawnPet(manager, 5330.0f, 522.0f);
    spawnPet(manager, 3490.0f, 1879.0f);
    spawnPet(manager, 4066.0f, 4302.0f);
    spawnPet(manager, 1361.0f, 4667.0f);
}



const int START_LEFT   = 237;
const int START_RIGHT  = 563;
const int START_TOP    = 200;
const int START_BOTTOM = 300;

const int QUIT_LEFT    = 238;
const int QUIT_RIGHT   = 564;
const int QUIT_TOP     = 455;
const int QUIT_BOTTOM  = 550;

void Game::handleEvents() {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;

            case SDL_MOUSEMOTION:
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                if (inMenu) {
                    std::cout << "Menu Mouse: (" << mouseX << ", " << mouseY << ")\n";
                }
                break;

            case SDL_KEYDOWN:
                if (inMenu && event.key.keysym.sym == SDLK_SPACE) {
                    std::cout << "Space pressed—ignoring because start button must be clicked.\n";
                }
                break;

            case SDL_MOUSEBUTTONDOWN: {
    if (inMenu) {
        // Get the current window size.
        int winW, winH;
        SDL_GetWindowSize(window, &winW, &winH);

        // Calculate scaling factors from actual window size to logical resolution (800x640).
        float scaleX = 800.0f / winW;
        float scaleY = 640.0f / winH;

        // Convert raw mouse click coordinates to logical coordinates.
        int clickX = static_cast<int>(event.button.x * scaleX);
        int clickY = static_cast<int>(event.button.y * scaleY);

        std::cout << "Mouse click at logical coordinates: ("
                  << clickX << ", " << clickY << ")\n";

        // Check if click is within the start button region.
        if (clickX > START_LEFT && clickX < START_RIGHT &&
            clickY > START_TOP  && clickY < START_BOTTOM)
        {
            std::cout << "Click inside start button! Starting game...\n";
            inMenu = false;
            initializeGame();
        }
        // Check if click is within the quit button region.
        else if (clickX > QUIT_LEFT && clickX < QUIT_RIGHT &&
                 clickY > QUIT_TOP  && clickY < QUIT_BOTTOM)
        {
            std::cout << "Click inside quit button! Quitting game...\n";
            isRunning = false;    // Set your game running flag to false so the main loop ends.
            // Optionally, call SDL_Quit() and/or clean up resources here.
        }
        else {
            std::cout << "Click outside valid buttons. Game remains in menu.\n";
        }
    }
    else {
        // When not in menu: Process bullet spawning as before.

        // Convert mouse coordinates (screen) to world coordinates using camera offset.
        int mouseX = event.button.x + camera.x;
        int mouseY = event.button.y + camera.y;

        // Get player's center from its TransformComponent.
        auto& playerTransform = player->getComponent<TransformComponent>();
        float playerCenterX = playerTransform.position.x + playerTransform.width / 2;
        float playerCenterY = playerTransform.position.y + playerTransform.height / 2;

        // Compute direction vector from player center to mouse click.
        float dx = mouseX - playerCenterX;
        float dy = mouseY - playerCenterY;
        float length = std::sqrt(dx * dx + dy * dy);
        Vector2D direction = { 0, 0 };
        if (length != 0) {
            direction.x = dx / length;
            direction.y = dy / length;
        }

        // Calculate spawn offset so bullet appears away from the player.
        float spawnOffset = playerTransform.width / 2 + 10;
        float bulletStartX = playerCenterX + direction.x * spawnOffset;
        float bulletStartY = playerCenterY + direction.y * spawnOffset;

        // Create a bullet entity.
        auto& bullet = manager.addEntity();
        bullet.addComponent<TransformComponent>(bulletStartX, bulletStartY, 8, 8, 1);
        bullet.addComponent<SpriteComponent>("Assets/Bullet.png");
        bullet.addComponent<BulletComponent>(direction, 10.0f, 300.0f);
    }
    break;


}case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int newWidth = event.window.data1;
                    int newHeight = event.window.data2;
                    SDL_RenderSetLogicalSize(renderer, newWidth, newHeight);
                    camera.w = newWidth;
                    camera.h = newHeight;
                }
                break;

            default:
                break;
        }
    }
}




void Game::update() {
    if (inMenu) {
        return; // Don't update game logic while in menu
    }
    Uint32 currentTime = SDL_GetTicks();

    // Log mouse position every second
    if (currentTime - lastMousePositionLogTime > mouseLogInterval) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Convert screen coordinates to world coordinates
        int worldX = mouseX + camera.x;
        int worldY = mouseY + camera.y;

        std::cout << "Mouse position - Screen: (" << mouseX << ", " << mouseY
                  << ") World: (" << worldX << ", " << worldY << ")" << std::endl;
        lastMousePositionLogTime = currentTime;
    }

    auto& playerTransform = player->getComponent<TransformComponent>();
    Vector2D oldPlayerPos = playerTransform.position;

    manager.refresh();
    manager.update();

    camera.x = static_cast<int>(playerTransform.position.x + playerTransform.width / 2 - camera.w / 2);
    camera.y = static_cast<int>(playerTransform.position.y + playerTransform.height / 2 - camera.h / 2);

    handleCollisions(oldPlayerPos);

    auto& playerCollider = player->getComponent<ColliderComponent>();

    // Pet collisions
    for (auto& col : colliders) {
        if (col->tag == "pet" && col->entity->isActive()) {
            if (Collision::AABB(playerCollider, *col)) {
                petCount++;
                std::cout << "Pet collected! Total pets: " << petCount << std::endl;
                col->entity->destroy();
            }
        }
    }

    // Enemy collisions
    bool collidedWithEnemy = false;
    for (auto& col : colliders) {
        if (col->tag == "enemy" && col->entity->isActive()) {
            if (Collision::AABB(playerCollider, *col)) {
                collidedWithEnemy = true;
                break;
            }
        }
    }
    if (collidedWithEnemy && (currentTime - lastHitTime >= 1000)) {
        heartCount--;
        lastHitTime = currentTime;
        if (heartCount <= 0) {
            isRunning = false;
        }
    }

    int tileX = static_cast<int>(playerTransform.position.x + playerTransform.width / 2) / 32;
    int tileY = static_cast<int>(playerTransform.position.y + playerTransform.height / 2) / 32;

    if (tileX >= 0 && tileX < 230 && tileY >= 0 && tileY < 230) {
        if (map->getTile(tileY, tileX) == 4 && petCount > 0) {
            storedPets += petCount;
            std::cout << "Stored " << petCount << " pets in the shelter!" << std::endl;
            std::cout << "Total stored pets: " << storedPets << std::endl;
            petCount = 0;
        }
    }
}





void Game::handleCollisions(Vector2D oldPlayerPos) {
    auto& playerTransform = player->getComponent<TransformComponent>();
    auto& playerCol = player->getComponent<ColliderComponent>();

    bool collisionX = false;
    bool collisionY = false;

    for (auto& cc : colliders) {
        if (cc->tag == "wall" && cc != &playerCol) {
            if (Collision::AABB(playerCol, *cc)) {
                // Test X-axis collision.
                Vector2D testPos = playerTransform.position;
                testPos.x = oldPlayerPos.x;

                SDL_Rect tempRect = playerCol.collider;
                playerCol.collider.x = static_cast<int>(testPos.x);
                if (Collision::AABB(playerCol.collider, cc->collider))
                    collisionY = true;

                playerCol.collider = tempRect;
                testPos = playerTransform.position;
                testPos.y = oldPlayerPos.y;
                playerCol.collider.y = static_cast<int>(testPos.y);
                if (Collision::AABB(playerCol.collider, cc->collider))
                    collisionX = true;
                playerCol.collider = tempRect;

                if (collisionX && collisionY)
                    playerTransform.position = oldPlayerPos;
                else if (collisionX)
                    playerTransform.position.x = oldPlayerPos.x;
                else if (collisionY)
                    playerTransform.position.y = oldPlayerPos.y;

                playerCol.update();
            }
        }
    }
}

void Game::render() {
    SDL_RenderClear(renderer);

    if (inMenu) {
        // Render menu background
        SDL_RenderCopy(renderer, menuTexture, NULL, NULL);

        // Render mouse position on screen (optional)
        if (showMousePosition) {
            std::string mousePosText = "Mouse: " + std::to_string(mouseX) + ", " + std::to_string(mouseY);
            // You'll need SDL_ttf for text rendering or use a simpler method
        }
    } else {
        // Render game
        map->DrawMap();
        manager.draw();
    }

    SDL_RenderPresent(renderer);
}

// Modify the clean function to clean up menu resources
void Game::clean() {
    if (menuTexture) {
        SDL_DestroyTexture(menuTexture);
    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    std::cout << "Game Cleaned!" << std::endl;
}


