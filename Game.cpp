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


Game::Game()
{}
Game::~Game()
{}


void Game::init(const char* title, int width, int height, bool fullscreen) {
    int flags = 0;
    if(fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if(SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        cout<<"Subsystems Initialized!!"<<endl;

        window = SDL_CreateWindow(title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, width, height, flags);
        renderer = SDL_CreateRenderer(window, -1, 0);
        if(window) {
            cout<<"Window Created!"<<endl;
        }

        if(renderer) {
            SDL_SetRenderDrawColor(renderer, 0,255,255,255);
            cout<<"Renderer Created!"<<endl;
        }

        isRunning = true;
    }
    map = new Map();

    tile0.addComponent<TileComponent>(200,200,32,32,0);
    player.addComponent<TransformComponent>(200.0f, 200.0f, 64, 64, 1);
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

        default:
            break;
    }
}

void Game::update() {
    Vector2D playerPos = player.getComponent<TransformComponent>().position;
    manager.refresh();
    manager.update();

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