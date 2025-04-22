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
#include <fstream>


#ifdef byte
#undef byte
#endif

Manager manager;
SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;
std::vector<ColliderComponent*> Game::colliders;
SDL_Rect Game::camera = { 0, 0, 800, 640 };
Entity* player = nullptr;
bool Game::inMenu = true;

SDL_Texture* Game::menuTexture = nullptr;
SDL_Texture* Game::winTexture = nullptr;
SDL_Texture* Game::pauseTexture = nullptr;

Game::Game() : isRunning(false), window(nullptr), map(nullptr) { }
Game::~Game() { }

void spawnEnemy(Manager& manager, float x, float y) {
    auto& enemy = manager.addEntity();
    // DODAS ENEMIJU TRANSFORM VELIKOST IN HITROST
    enemy.addComponent<TransformComponent>(x, y, 72, 72, 1); // HITROST JE 1
    enemy.addComponent<SpriteComponent>("Assets/Nasprotnik.png");
    enemy.addComponent<ColliderComponent>("enemy");
    // TUKAJ SPREMENIS FOLLOW SPEED ZA ENEMY
    enemy.addComponent<EnemyAIComponent>(320.0f, 0.5f);
}

// POMOŽNA FUNKCIJA ZA PET SPAWN
void spawnPet(Manager& manager, float x, float y) {
    auto& pet = manager.addEntity();
    // DOLOČIŠ SIZE 48X48
    pet.addComponent<TransformComponent>(x, y, 48, 48, 1);
    // DODAS SPRITE ZA PET
    pet.addComponent<SpriteComponent>("Assets/Zival.png");
    // DODAJ COLLIDER Z TAGOM PET
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
    // MENU TEXTURA
    menuTexture = TextureManager::LoadTexture("Assets/Menu.png");
    if (!menuTexture) {
        std::cout << "Failed to load menu texture!" << std::endl;
    }
    // ZMAGA TEXTURA
    winTexture = TextureManager::LoadTexture("Assets/WinScreen.png");
    if (!winTexture) {
        std::cout << "Failed to load win screen texture!" << std::endl;
    }
    //PAUSE TEXTURA
    pauseTexture = TextureManager::LoadTexture("Assets/PauseScreen.png");
    if (!pauseTexture) {
        std::cout << "Failed to load pause screen texture!" << std::endl;
    }
    // GAME STATE
    inMenu = true;
}


void Game::initializeGame() {
    recorder.begin();
    // Naredis mapo in colliderje
    map = new Map();
    map->LoadMap();
    map->CreateWallColliders(manager);

    // Spawn playerja
    // Ce ni klika bo osalo na 200 200
    player = &manager.addEntity();
    player->addComponent<TransformComponent>(playerStartX, playerStartY, 40, 42, 1);
    player->addComponent<SpriteComponent>("Assets/Clovek.png");
    player->addComponent<KeyboardController>();
    player->addComponent<ColliderComponent>("player");


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


    petCount = 0;
    heartCount = 3;
    lastHitTime = 0;
    storedPets = 7;

    spawnPet(manager, 6563.0f, 4801.0f);
    spawnPet(manager, 6156.0f, 4520.0f);
    spawnPet(manager, 6119.0f, 1555.0f);
    spawnPet(manager, 5825.0f, 886.0f);
    spawnPet(manager, 5330.0f, 522.0f);
    spawnPet(manager, 3490.0f, 1879.0f);
    spawnPet(manager, 4066.0f, 4302.0f);
    spawnPet(manager, 1361.0f, 4667.0f);
    spawnPet(manager, 436.0f, 206.0f);
}



const int START_LEFT   = 237;
const int START_RIGHT  = 563;
const int START_TOP    = 200;
const int START_BOTTOM = 300;

const int QUIT_LEFT    = 238;
const int QUIT_RIGHT   = 564;
const int QUIT_TOP     = 455;
const int QUIT_BOTTOM  = 550;

const int PAUSE_QUIT_LEFT   = 230;
const int PAUSE_QUIT_RIGHT  = 571;
const int PAUSE_QUIT_TOP    = 360;
const int PAUSE_QUIT_BOTTOM = 459;

const int WIN_QUIT_LEFT   = 232;
const int WIN_QUIT_RIGHT  = 566;
const int WIN_QUIT_TOP    = 334;
const int WIN_QUIT_BOTTOM = 447;

const int PAUSE_SAVE_LEFT   = 229;
const int PAUSE_SAVE_RIGHT  = 571;
const int PAUSE_SAVE_TOP    = 183;
const int PAUSE_SAVE_BOTTOM = 264;

const int LOAD_LEFT   = 237;
const int LOAD_RIGHT  = 563;
const int LOAD_TOP    = 328;
const int LOAD_BOTTOM = 425;

const int WIN_REPLAY_LEFT   = 234;
const int WIN_REPLAY_RIGHT  = 569;
const int WIN_REPLAY_TOP    = 205;
const int WIN_REPLAY_BOTTOM = 318;

void Game::handleEvents() {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                saveGame();
                isRunning = false;
                break;

            case SDL_MOUSEMOTION:
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                if (inMenu) {
                    std::cout << "Menu Mouse: (" << mouseX << ", " << mouseY << ")\n";
                }
                else if (gameWon) {
                    std::cout << "Win Screen Mouse: (" << mouseX << ", " << mouseY << ")\n";
                }
                break;

            case SDL_KEYDOWN:
                if (inMenu && event.key.keysym.sym == SDLK_SPACE) {
                    // ignore
                }
                else if (!inMenu && !gameWon && event.key.keysym.sym == SDLK_ESCAPE) {
                    isPaused = !isPaused;
                    std::cout << (isPaused ? "Game paused." : "Game unpaused.") << "\n";
                }
                break;

            case SDL_MOUSEBUTTONDOWN: {
                int winW, winH;
                SDL_GetWindowSize(window, &winW, &winH);
                float scaleX = 800.0f / winW;
                float scaleY = 640.0f / winH;
                int clickX = static_cast<int>(event.button.x * scaleX);
                int clickY = static_cast<int>(event.button.y * scaleY);

                // --- MENU ---
                if (inMenu) {
                    if (clickX > START_LEFT && clickX < START_RIGHT &&
                        clickY > START_TOP  && clickY < START_BOTTOM)
                    {
                        inMenu = false;
                        initializeGame();
                    }
                    else if (clickX > QUIT_LEFT && clickX < QUIT_RIGHT &&
                             clickY > QUIT_TOP  && clickY < QUIT_BOTTOM)
                    {
                        saveGame();
                        isRunning = false;
                    }
                    else if (clickX > LOAD_LEFT && clickX < LOAD_RIGHT &&
                             clickY > LOAD_TOP  && clickY < LOAD_BOTTOM)
                    {
                        loadGame();
                    }
                    return;
                }

                // --- PAUSE ---
                if (isPaused) {
                    if (clickX > PAUSE_QUIT_LEFT && clickX < PAUSE_QUIT_RIGHT &&
                        clickY > PAUSE_QUIT_TOP   && clickY < PAUSE_QUIT_BOTTOM)
                    {
                        saveGame();
                        isRunning = false;
                    }
                    else if (clickX > PAUSE_SAVE_LEFT && clickX < PAUSE_SAVE_RIGHT &&
                             clickY > PAUSE_SAVE_TOP   && clickY < PAUSE_SAVE_BOTTOM)
                    {
                        saveGame();
                    }
                    return;
                }

                // --- WIN & REPLAY ---
                if (gameWon) {
                    std::cout << "[DEBUG] Win click: (" << clickX << ", " << clickY << ")\n";

                    if (clickX > WIN_QUIT_LEFT && clickX < WIN_QUIT_RIGHT &&
                        clickY > WIN_QUIT_TOP   && clickY < WIN_QUIT_BOTTOM)
                    {
                        saveGame();
                        isRunning = false;
                    }
                    else if (clickX > WIN_REPLAY_LEFT && clickX < WIN_REPLAY_RIGHT &&
                             clickY > WIN_REPLAY_TOP   && clickY < WIN_REPLAY_BOTTOM)
                    {
                        std::cout << "[DEBUG] Replay button hit!\n";
                        gameWon     = false;
                        manager     = Manager();
                        Game::colliders.clear();
                        isReplaying = true;

                        if (!replayer.begin()) {
                            std::cerr << "Failed to open replay.csv\n";
                            isReplaying = false;
                            return;
                        }

                        // ghost player
                        replayPlayer = &manager.addEntity();
                        replayPlayer->addComponent<TransformComponent>(0,0,40,42,1);
                        replayPlayer->addComponent<SpriteComponent>("Assets/Clovek.png");

                        // ghost enemies
                        replayEnemies.clear();
                        for (int i = 0; i < 20; ++i) {
                            auto& e = manager.addEntity();
                            e.addComponent<TransformComponent>(0,0,72,72,1);
                            e.addComponent<SpriteComponent>("Assets/Nasprotnik.png");
                            replayEnemies.push_back(&e);
                        }

                        // ghost pets
                        replayPets.clear();
                        for (int i = 0; i < 20; ++i) {
                            auto& p = manager.addEntity();
                            p.addComponent<TransformComponent>(0,0,48,48,1);
                            p.addComponent<SpriteComponent>("Assets/Zival.png");
                            replayPets.push_back(&p);
                        }

                        // ghost bullets
                        replayBullets.clear();
                        for (int i = 0; i < 50; ++i) {
                            auto& b = manager.addEntity();
                            b.addComponent<TransformComponent>(0,0,8,8,1);
                            b.addComponent<SpriteComponent>("Assets/Bullet.png");
                            replayBullets.push_back(&b);
                        }
                    }
                    return;
                }

                // --- GAMEPLAY SHOOTING ---
                {
                    int worldX = event.button.x + camera.x;
                    int worldY = event.button.y + camera.y;
                    auto& ptx = player->getComponent<TransformComponent>();
                    float pcx = ptx.position.x + ptx.width*0.5f;
                    float pcy = ptx.position.y + ptx.height*0.5f;
                    float dx = worldX - pcx, dy = worldY - pcy;
                    float len = std::sqrt(dx*dx + dy*dy);
                    Vector2D dir{0,0};
                    if (len > 0) { dir.x = dx/len; dir.y = dy/len; }

                    float off = ptx.width/2 + 10;
                    auto& b = manager.addEntity();
                    b.addComponent<TransformComponent>(
                        pcx + dir.x*off,
                        pcy + dir.y*off,
                        8,8,1
                    );
                    b.addComponent<SpriteComponent>("Assets/Bullet.png");
                    b.addComponent<BulletComponent>(dir, 20.0f, 300.0f);
                }
                break;
            }

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int newW = event.window.data1;
                    int newH = event.window.data2;
                    SDL_RenderSetLogicalSize(renderer, newW, newH);
                    camera.w = newW;
                    camera.h = newH;
                }
                break;

            default:
                break;
        }
    }
}



void Game::update() {
    if (inMenu) return;

    if (isReplaying) {
        FrameData fd;
        if (!replayer.next(fd)) {
            replayer.end();
            isReplaying = false;
            return;
        }

        auto& rptx = replayPlayer->getComponent<TransformComponent>();
        rptx.position = fd.player;
        camera.x = int(rptx.position.x + rptx.width*0.5f - camera.w*0.5f);
        camera.y = int(rptx.position.y + rptx.height*0.5f - camera.h*0.5f);

        for (size_t i = 0; i < replayEnemies.size(); ++i) {
            auto* e = replayEnemies[i];
            auto& tx = e->getComponent<TransformComponent>();
            if (i < fd.enemies.size()) tx.position = fd.enemies[i];
            else                        tx.position = Vector2D{-1000,-1000};
        }
        for (size_t i = 0; i < replayPets.size(); ++i) {
            auto* p = replayPets[i];
            auto& tx = p->getComponent<TransformComponent>();
            if (i < fd.pets.size()) tx.position = fd.pets[i];
            else                     tx.position = Vector2D{-1000,-1000};
        }
        for (size_t i = 0; i < replayBullets.size(); ++i) {
            auto* b = replayBullets[i];
            auto& tx = b->getComponent<TransformComponent>();
            if (i < fd.bullets.size()) tx.position = fd.bullets[i];
            else                        tx.position = Vector2D{-1000,-1000};
        }

        return;
    }


    if (isPaused || gameWon) return;

    Uint32 now = SDL_GetTicks();
    if (now - lastMousePositionLogTime > mouseLogInterval) {
        int mx, my; SDL_GetMouseState(&mx, &my);
        lastMousePositionLogTime = now;
    }

    auto& ptx = player->getComponent<TransformComponent>();
    Vector2D oldPos = ptx.position;

    manager.refresh();
    manager.update();

    camera.x = int(ptx.position.x + ptx.width*0.5f - camera.w*0.5f);
    camera.y = int(ptx.position.y + ptx.height*0.5f - camera.h*0.5f);

    handleCollisions(oldPos);

    auto& pcol = player->getComponent<ColliderComponent>();
    for (auto& c : colliders) {
        if (c->tag=="pet" && c->entity->isActive() &&
            Collision::AABB(pcol,*c)) {
            petCount++;
            c->entity->destroy();
        }
    }

    bool hit=false;
    for (auto& c:colliders) {
        if (c->tag=="enemy" && c->entity->isActive() &&
            Collision::AABB(pcol,*c)) { hit=true; break; }
    }
    if (hit && now - lastHitTime>=1000) {
        heartCount--; lastHitTime=now;
        if (heartCount<=0) isRunning=false;
    }

    int tx = int(ptx.position.x + ptx.width*0.5f)/32;
    int ty = int(ptx.position.y + ptx.height*0.5f)/32;
    if (tx>=0 && tx<230 && ty>=0 && ty<230 &&
        map->getTile(ty,tx)==4 && petCount>0)
    {
        storedPets+=petCount;
        petCount=0;
        if (storedPets>=8) gameWon=true;
    }

    FrameData fd;
    fd.player = ptx.position;
    for (auto& c:colliders) {
        if (c->tag=="enemy" && c->entity->isActive())
            fd.enemies.push_back(c->entity->getComponent<TransformComponent>().position);
        if (c->tag=="pet" && c->entity->isActive())
            fd.pets.push_back(c->entity->getComponent<TransformComponent>().position);
        if (c->tag=="bullet" && c->entity->isActive())
            fd.bullets.push_back(c->entity->getComponent<TransformComponent>().position);
    }
    recorder.recordFrame(fd);
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
    // 1) Clear the back buffer
    SDL_RenderClear(renderer);

    // 2) REPLAY MODE: draw the map, update sprites, then draw ghosts
    if (isReplaying) {
        if (map) {
            map->DrawMap();
        }
        // run the per‐frame update on each component (so SpriteComponent sets destRect)
        manager.update();
        // now draw your ghost entities
        manager.draw();
        SDL_RenderPresent(renderer);
        return;
    }

    // 3) WIN SCREEN
    if (gameWon) {
        SDL_RenderCopy(renderer, winTexture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        return;
    }

    // 4) MAIN MENU
    if (inMenu) {
        SDL_RenderCopy(renderer, menuTexture, nullptr, nullptr);
    }
    else {
        // 5) NORMAL GAMEPLAY
        if (map) {
            map->DrawMap();
        }
        manager.draw();
    }

    // 6) PAUSE OVERLAY
    if (isPaused) {
        SDL_RenderCopy(renderer, pauseTexture, nullptr, nullptr);
    }

    // 7) Present the frame
    SDL_RenderPresent(renderer);
}




void Game::saveGame() {
    std::ofstream saveFile("savegame.txt");
    if (saveFile.is_open()) {
        auto& playerTransform = player->getComponent<TransformComponent>();

        saveFile << "petCount " << petCount << "\n";
        saveFile << "storedPets " << storedPets << "\n";
        saveFile << "heartCount " << heartCount << "\n";
        saveFile << "playerX " << playerTransform.position.x << "\n";
        saveFile << "playerY " << playerTransform.position.y << "\n";

        // Save enemy positions
        for (auto& col : Game::colliders) {
            if (col->tag == "enemy" && col->entity->isActive()) {
                auto& enemyTransform = col->entity->getComponent<TransformComponent>();
                saveFile << "enemy " << enemyTransform.position.x << " "
                         << enemyTransform.position.y << "\n";
            }
        }

        saveFile.close();
        std::cout << "Game saved successfully.\n";
    } else {
        std::cout << "Failed to open save file.\n";
    }
}

void Game::loadGame() {
    std::ifstream loadFile("savegame.txt");
    if (!loadFile.is_open()) {
        std::cout << "No save file found.\n";
        return;
    }

    int loadedPetCount = 0, loadedStoredPets = 0, loadedHeartCount = 3;
    float playerX = 200.0f, playerY = 200.0f;
    std::vector<Vector2D> enemyPositions;

    std::string label;
    while (loadFile >> label) {
        if (label == "petCount") loadFile >> loadedPetCount;
        else if (label == "storedPets") loadFile >> loadedStoredPets;
        else if (label == "heartCount") loadFile >> loadedHeartCount;
        else if (label == "playerX") loadFile >> playerX;
        else if (label == "playerY") loadFile >> playerY;
        else if (label == "enemy") {
            float x, y;
            loadFile >> x >> y;
            enemyPositions.emplace_back(x, y);
        }
    }
    loadFile.close();

    std::cout << "Save file loaded.\n";

    // Apply loaded values
    petCount = loadedPetCount;
    storedPets = loadedStoredPets;
    heartCount = loadedHeartCount;
    playerStartX = static_cast<int>(playerX);
    playerStartY = static_cast<int>(playerY);

    inMenu = false;

    // Start fresh
    initializeGame();

    // Remove all default enemies that were spawned
    for (auto& col : Game::colliders) {
        if (col->tag == "enemy") {
            col->entity->destroy();
        }
    }

    // Re-spawn loaded enemies
    for (const auto& pos : enemyPositions) {
        spawnEnemy(manager, pos.x, pos.y);
    }
}


// Modify the clean function to clean up menu resources
void Game::clean() {
    recorder.end();
    if (menuTexture) {
        SDL_DestroyTexture(menuTexture);
    }
    if (winTexture) {
        SDL_DestroyTexture(winTexture);
    }
    if (pauseTexture) {
        SDL_DestroyTexture(pauseTexture);
    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    std::cout << "Game Cleaned!" << std::endl;
}


