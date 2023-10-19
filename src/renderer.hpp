#ifndef RENDERER_H
#define RENDERER_H
#include "raylib.h"
#include <iostream>
#include <vector>
#include "Entity.hpp"
#include "simulator.hpp"

class Renderer
{
private:
    Vector2 windowsSize = Vector2();

public:
    float scale = 1.0 / 1000.0f;

    Renderer(/* args */);
    ~Renderer();

    void setScale(float scale) { this->scale = scale; };
    void setWindowsSize(float x, float y) { this->windowsSize = {x, y}; };

    void render(Simulator *sim);
    void mainLoop();
};

Renderer::Renderer(/* args */)
{
}

Renderer::~Renderer()
{
}

void Renderer::render(Simulator *sim)
{
    Vector3 center = {
        sim->origin->position.x,
        sim->origin->position.y,
        0
    };
    BeginDrawing();
    for (Entity *entity : sim->entities)
    {
        DrawCircle(
            (center.x - entity->position.x) * scale + windowsSize.x / 2,
            (center.y - entity->position.y) * scale + windowsSize.y / 2,
            10,
            WHITE);
    }
    EndDrawing();
}

#endif