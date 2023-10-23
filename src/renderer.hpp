#ifndef RENDERER_H
#define RENDERER_H
#include "raylib.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include "Entity.hpp"
#include "simulator.hpp"

class Renderer
{
private:
    Vector2 windowsSize = Vector2();

public:
    float scale = 1.0 / 2000.0f;

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
    Vector3l center = {
        sim->origin->position.x,
        // 0,
        sim->origin->position.y,
        // 0,
        0};
    
    ClearBackground(BLACK);
    for (Entity *entity : sim->entities)
    {
        DrawCircle(
            round((center.x - entity->position.x) * -scale + windowsSize.x / 2),
            round((center.y - entity->position.y) * -scale + windowsSize.y / 2),
            entity->radius * scale,
            entity->color);

        auto points = sim->lines.find((size_t)entity)->second;
        if (points->size() >= 2)
        {
            for (size_t i = 0; i < points->size() - 1; i += 1)
            {
                Color color = ColorAlpha(entity->color, 0.6 * i / (points->size() - 1));
                DrawLine(
                    round((center.x - points->at(i).x) * -scale + windowsSize.x / 2),
                    round((center.y - points->at(i).y) * -scale + windowsSize.y / 2),
                    round((center.x - points->at(i + 1).x) * -scale + windowsSize.x / 2),
                    round((center.y - points->at(i + 1).y) * -scale + windowsSize.y / 2),
                    color
                );
            }
            Color color = ColorAlpha(entity->color, 0.6);
            DrawLine(
                 round((center.x - points->back().x) * -scale + windowsSize.x / 2),
                 round((center.y - points->back().y) * -scale + windowsSize.y / 2),
                 round((center.x - entity->position.x) * -scale + windowsSize.x / 2),
                 round((center.y - entity->position.y) * -scale + windowsSize.y / 2),
                 color
            );
        }
    }
}

#endif