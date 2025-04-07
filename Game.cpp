#include "Game.h"
#include "TextureManager.h"
#include "ESC/ECS.h"
#include "ESC/Components.h"
#include "Map.h"
#include "Vector2D.h"
#include "Collision.h"
#include "ESC/ColliderComponent.h"
using namespace std;

Map* map;
Manager manager;

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;
vector<ColliderComponent*> Game::colliders;
auto& player(manager.addEntity());
auto& tile0(manager.addEntity());
auto& tile1(manager.addEntity());

SDL_Rect Game::camera = { 0, 0, 800, 640 };
Game::Game()
{}
Game::~Game()
{}


void Game::init(const char* title, int width, int height, bool fullscreen) {
    int flags = 0;
    if(fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    } else {
        flags = SDL_WINDOW_RESIZABLE;  // Make the window resizable
    }

    if(SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        cout << "Subsystems Initialized!!" << endl;
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
        renderer = SDL_CreateRenderer(window, -1, 0);
        if(window) {
            cout << "Window Created!" << endl;
        }
        if(renderer) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            SDL_RenderSetLogicalSize(renderer, 800, 640);  // Set your desired logical size here
            cout << "Renderer Created!" << endl;
        }
        isRunning = true;
    }

    map = new Map();
    map->LoadMap();
    map->CreateWallColliders(manager);

    // Setup player entity with components
    player.addComponent<TransformComponent>(200.0f, 200.0f, 40, 43, 1);
    player.addComponent<SpriteComponent>("Assets/Clovek.png");
    player.addComponent<KeyboardController>();
    player.addComponent<ColliderComponent>("player");
}

void Game::handleEvents() {
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT:
            isRunning = false;
        break;
        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                int newWidth = event.window.data1;
                int newHeight = event.window.data2;
                SDL_RenderSetLogicalSize(renderer, newWidth, newHeight);

                // Update the camera dimensions to match the new logical size
                Game::camera.w = newWidth;
                Game::camera.h = newHeight;
            }
        break;
        default:
            break;
    }
}

void Game::update() {
    auto& playerTransform = player.getComponent<TransformComponent>();
    Vector2D oldPlayerPos = playerTransform.position;

    manager.refresh();
    manager.update();

    // Update camera to center on the player
    Game::camera.x = static_cast<int>(playerTransform.position.x + playerTransform.width / 2 - Game::camera.w / 2);
    Game::camera.y = static_cast<int>(playerTransform.position.y + playerTransform.height / 2 - Game::camera.h / 2);

    // Optional: Clamp camera values so it doesn’t go out of your map boundaries

    handleCollisions(oldPlayerPos);
}

void Game::handleCollisions(Vector2D oldPlayerPos) {
    auto& playerTransform = player.getComponent<TransformComponent>();
    auto& playerCol = player.getComponent<ColliderComponent>();

    bool collisionX = false;
    bool collisionY = false;


    for(auto& cc : colliders) {
        if(cc->tag == "wall" && cc != &playerCol) {

            if(Collision::AABB(playerCol, *cc)) {


                // Test X-axis collision
                Vector2D testPos = playerTransform.position;
                testPos.x = oldPlayerPos.x;


                SDL_Rect tempRect = playerCol.collider;
                playerCol.collider.x = static_cast<int>(testPos.x);


                if(Collision::AABB(playerCol.collider, cc->collider)) {
                    collisionY = true;
                }


                playerCol.collider = tempRect;
                testPos = playerTransform.position;
                testPos.y = oldPlayerPos.y;

                playerCol.collider.y = static_cast<int>(testPos.y);


                if(Collision::AABB(playerCol.collider, cc->collider)) {
                    collisionX = true;
                }

                playerCol.collider = tempRect;

                if(collisionX && collisionY) {
                    playerTransform.position = oldPlayerPos;
                } else if(collisionX) {
                    playerTransform.position.x = oldPlayerPos.x;
                } else if(collisionY) {
                    playerTransform.position.y = oldPlayerPos.y;
                }

                playerCol.update();
            }
        }
    }
}

void Game::render() {
    SDL_RenderClear(renderer);
    map->DrawMap();
    manager.draw();
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    cout<<"Game Cleaned!"<<endl;
}