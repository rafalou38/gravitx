#ifndef RENDERER_H
#define RENDERER_H
#include "raylib.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include "Entity.hpp"
#include "simulator.hpp"
#include "raymath.h"

struct Polar3D{
    float d;
    float theta;
    float phi;
};

class Renderer
{
private:
#ifdef D3D
    Camera3D camera;
    Polar3D cameraPos;
#endif

    Vector2 windowsSize = Vector2();

public:
    float scale = 1.0 / 2000.0f;

    Renderer(/* args */);
    ~Renderer();

    void setScale(float scale) { this->scale = scale; };
    void setWindowsSize(float x, float y) { this->windowsSize = {x, y}; };

    void update();
    void updateCameraPos();

    void render(Simulator *sim);
    void render3D(Simulator *sim);

    void mainLoop();
};

Renderer::Renderer(/* args */)
{
    cameraPos = {
        .d = 100,
        .theta = 0,
        .phi = PI/4
    };
    camera = {0};

    camera.target = (Vector3){0.0f, 0.0f, 0.0f};      // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                              // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;           // Camera projection type

    updateCameraPos();
}

Renderer::~Renderer()
{
}

void Renderer::updateCameraPos(){
    // camera.position.x  = cameraPos.d * cos(cameraPos.phi) * cos(cameraPos.theta);
    // camera.position.y  = cameraPos.d * cos(cameraPos.phi) * sin(cameraPos.theta);
    // camera.position.z  = cameraPos.d * sin(cameraPos.phi);
    camera.position.x  = 0;
    camera.position.y  = cameraPos.d * sin(cameraPos.phi);
    camera.position.z  = cameraPos.d * cos(cameraPos.phi);

    cout << " x:" << camera.position.x;
    cout << " y:" << camera.position.y;
    cout << " z:" << camera.position.z << endl;
}
void Renderer::update()
{
#ifdef D3D
    // UpdateCamera(&camera, CAMERA_CUSTOM);
    if(IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePositionDelta = GetMouseDelta();
        // cameraPos.phi +=  mousePositionDelta.y / windowsSize.y * 2 * PI;
        cameraPos.theta +=  mousePositionDelta.x / windowsSize.x * 2 * PI;

        cout << "theta:" << cameraPos.theta << endl;
        // cameraPos.phi += -1 *  mousePositionDelta.y / windowsSize.y * 2 * PI;
        // cameraPos.theta += 0.01;
        updateCameraPos();
    }

#endif
}

void Renderer::render3D(Simulator *sim)
{


    Vector3l center = {
        sim->origin->position.x,
        // 0,
        sim->origin->position.y,
        // 0,
        sim->origin->position.z};
    ClearBackground(BLACK);
    BeginMode3D(camera);
    DrawGrid(20, scale * 100000.0f);

    DrawLine3D({0,0,0}, {100,0,0}, RED);
    DrawLine3D({0,0,0}, {0,100,0}, GREEN);
    DrawLine3D({0,0,0}, {0,0,100}, BLUE);

    Vector3 pos;
    for (Entity *entity : sim->entities)
    {
        pos = Vector3{
            (float)((center.x - entity->position.x) * -scale),
            (float)((center.y - entity->position.y) * -scale),
            (float)((center.z - entity->position.z) * -scale)};
        DrawSphere(
            pos,
            entity->radius * scale,
            entity->color);
        auto points = sim->lines.find((size_t)entity)->second;
        if (points->size() >= 2)
        {
            Vector3 startPos;
            Vector3 endPos;
            for (size_t i = 0; i < points->size() - 1; i += 1)
            {
                startPos = {
                    (float)((center.x - points->at(i).x) * -scale),
                    (float)((center.y - points->at(i).y) * -scale),
                    (float)((center.z - points->at(i).z) * -scale),
                };
                endPos = {
                    (float)((center.x - points->at(i + 1).x) * -scale),
                    (float)((center.y - points->at(i + 1).y) * -scale),
                    (float)((center.z - points->at(i + 1).z) * -scale),
                };
                Color color = ColorAlpha(entity->color, 0.6 * i / (points->size() - 1));
                DrawLine3D(
                    startPos,
                    endPos,
                    color);
            }
            Color color = ColorAlpha(entity->color, 0.6);

            startPos = {
                (float)((center.x - points->back().x) * -scale),
                (float)((center.y - points->back().y) * -scale),
                (float)((center.z - points->back().z) * -scale),
            };
            endPos = {
                (float)((center.x - entity->position.x) * -scale),
                (float)((center.y - entity->position.y) * -scale),
                (float)((center.z - entity->position.z) * -scale),
            };

            DrawLine3D(startPos, endPos, color);
        }
    }

    EndMode3D();
}
void Renderer::render(Simulator *sim)
{
    Vector3l center = {
        sim->origin->position.x,
        // 0,
        sim->origin->position.y,
        // 0,
        0};
    // cout << sim->entities.size() << endl;
    ClearBackground(BLACK);
    for (Entity *entity : sim->entities)
    {
        if (entity->texturePath == "")
        {
            DrawCircle(
                round((center.x - entity->position.x) * -scale + windowsSize.x / 2),
                round((center.y - entity->position.y) * -scale + windowsSize.y / 2),
                entity->radius * scale,
                entity->color);
        }
        else
        {
            auto texture = entity->getTexture();
            float textureScale = (entity->radius * scale * 2.0f) / texture.width;
            DrawTextureEx(
                texture,
                {
                    (float)round((center.x - entity->position.x) * -scale + windowsSize.x / 2) - entity->radius * scale,
                    (float)round((center.y - entity->position.y) * -scale + windowsSize.y / 2) - entity->radius * scale,
                },
                0, textureScale, WHITE);
        }

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
                    color);
            }
            Color color = ColorAlpha(entity->color, 0.6);
            DrawLine(
                round((center.x - points->back().x) * -scale + windowsSize.x / 2),
                round((center.y - points->back().y) * -scale + windowsSize.y / 2),
                round((center.x - entity->position.x) * -scale + windowsSize.x / 2),
                round((center.y - entity->position.y) * -scale + windowsSize.y / 2),
                color);
        }
    }
}

#endif