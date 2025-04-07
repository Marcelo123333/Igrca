#pragma once
#include "Components.h"
#include "../Vector2D.h"

class TransformComponent : public Component {

public:

    Vector2D position;
    Vector2D velocity;

    int speed = 3;
    int height = 64;
    int width = 64;
    int scale = 1;


    TransformComponent() {
        position.Zero();
    }

    TransformComponent(int sc) {
        position.Zero();
        scale = sc;
    }

    TransformComponent(float x, float y) {
        position.Zero();
    }

    TransformComponent(float x, float y, int h, int w, int sc) {
        position.x = x;
        position.y = y;
        height = h;
        width = w;
        scale = sc;
    }

    void init() override {
        velocity.Zero();
    }
    void update() override {
        position.x += velocity.x * speed;
        position.y += velocity.y * speed;
    }

};