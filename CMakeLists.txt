cmake_minimum_required(VERSION 3.29)
project(Igrica)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(SDL2_PATH "C:/Users/HELP404/libs/SDL2-devel-2.28.0-mingw/SDL2-2.28.0/x86_64-w64-mingw32")

set(SDL2_IMAGE_PATH "C:/Users/HELP404/CLionProjects/Igrica/SDL2_image-devel-2.8.4-mingw/SDL2_image-2.8.4")
find_package(SDL2_image REQUIRED)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

add_executable(Igrica main.cpp Game.cpp Game.h
        TextureManager.h
        TextureManager.cpp
        GameObject.h
        GameObject.cpp
        ESC/Components.h
        ESC/ECS.h
        Map.h
        Map.cpp
        ESC/SpriteComponent.h
        ESC/TransformComponent.h
        Vector2D.h
        Vector2D.cpp
        ESC/KeyboardController.h
        ESC/ColliderComponent.h
        Collision.h
        Collision.cpp
        ESC/TileComponent.h
        ESC/ColliderComponent.cpp
        ESC/SpriteComponent.cpp
        BulletComponent.h
        ESC/EnemyAIComponent.h
        ESC/EnemyAIComponent.cpp
)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES})
