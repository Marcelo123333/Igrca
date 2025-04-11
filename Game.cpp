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
void Game::initializeGame() {
    // Create our map and wall colliders.
    map = new Map();
    map->LoadMap();
    map->CreateWallColliders(manager);

    // Spawn the player and assign it to the global player pointer.
    player = &manager.addEntity();
    player->addComponent<TransformComponent>(200.0f, 200.0f, 40, 43, 1);
    player->addComponent<SpriteComponent>("Assets/Clovek.png");
    player->addComponent<KeyboardController>();
    player->addComponent<ColliderComponent>("player");

    // Spawn enemies
    spawnEnemy(manager, 1330.0f, 395.0f);
    spawnEnemy(manager, 1925.0f, 1005.0f);
    spawnEnemy(manager, 1386.0f, 1035.0f);
    spawnEnemy(manager, 1025.0f, 1740.0f);
    spawnEnemy(manager, 1510.0f, 2635.0f);
    spawnEnemy(manager, 1960.0f, 2305.0f);
    spawnEnemy(manager, 95.0f, 2860.0f);
    spawnEnemy(manager, 390.0f, 3035.0f);
    spawnEnemy(manager, 325.0f, 1245.0f);
    spawnEnemy(manager, 205.0f, 1960.0f);

    // Initialize game stats
    petCount = 0;
    heartCount = 3;
    lastHitTime = 0;

    // Spawn pets
    spawnPet(manager, 80.0f, 3045.0f);
    spawnPet(manager, 2805.0f, 2670.0f);
}
    void Game::handleEvents() {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                break;

                case SDL_MOUSEMOTION:
                    // Update mouse position
                        mouseX = event.motion.x;
                mouseY = event.motion.y;

                if (inMenu) {
                    // Continuous mouse position output in menu
                    std::cout << "Menu Mouse: (" << mouseX << ", " << mouseY << ")\n";
                }
                break;
            case SDL_KEYDOWN:
                if (inMenu && event.key.keysym.sym == SDLK_SPACE) {
                    inMenu = false;
                    // Initialize the actual game when leaving menu
                    initializeGame();
                }
            break;
            case SDL_MOUSEBUTTONDOWN: {
                // Convert mouse coordinates (screen) to world coordinates by adding camera offset.
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
                break;
            }
            case SDL_WINDOWEVENT:
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


