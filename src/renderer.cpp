#include <iostream>
#include <vector>
#include <unordered_map>

#include "raylib.h"
#include "raymath.h"

#include "Entity.hpp"
#include "simulator.hpp"
#include "UI.hpp"
#include "gravitx.hpp"

#include "renderer.hpp"

Renderer::Renderer(AppComponents components)
{
    this->components = components;

#ifdef D3D
    resetCamera();
    camera = {0};

    camera.target = (Vector3){0.0f, 0.0f, 0.0f}; // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};     // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                         // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;      // Camera projection type

    updateCameraPos();
#endif
}

Renderer::~Renderer()
{
}
void Renderer::resetCamera(){
    cameraPos = {
        .d = 100,
        .theta = DEG2RAD * -270,
        .phi = DEG2RAD * 270};
}

#ifdef D3D
void Renderer::updateCameraPos()
{
    cameraFlipped = fmod(abs(cameraPos.phi), 2 * PI) > PI;

    camera.position.x = cameraPos.d * cos(cameraPos.theta) * sin(cameraPos.phi);
    camera.position.y = cameraPos.d * cos(cameraPos.phi);
    camera.position.z = cameraPos.d * sin(cameraPos.theta) * sin(cameraPos.phi);

    if (cameraFlipped)
        camera.up.y = -1;
    else
        camera.up.y = 1;
}
#endif

void Renderer::update()
{
#ifdef D3D
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !components.ui->isCursorInWindow())
    {
        Vector2 mousePositionDelta = GetMouseDelta();
        
        if (cameraFlipped)
            cameraPos.theta -= mousePositionDelta.x / windowsSize.x * 2 * PI;
        else
            cameraPos.theta += mousePositionDelta.x / windowsSize.x * 2 * PI;

        cameraPos.phi -= mousePositionDelta.y / windowsSize.y * 2 * PI;
        if (cameraPos.phi < 0)
            cameraPos.phi = 2 * PI + cameraPos.phi;

        updateCameraPos();
    }
#endif
}

#ifdef D3D
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

#ifdef GIZMOS
    // DrawGrid(20, scale * 100000.0f);
    DrawLine3D({0, 0, 0}, {100, 0, 0}, ColorAlpha(RED, 0.3));
    DrawLine3D({0, 0, 0}, {0, 100, 0}, ColorAlpha(GREEN, 0.3));
    DrawLine3D({0, 0, 0}, {0, 0, 100}, ColorAlpha(BLUE, 0.3));
#endif

    Vector3 pos;
    for (Entity *entity : sim->entities)
    {
        pos = Vector3{
            (float)((entity->position.x - center.x) * scale),
            (float)((entity->position.y - center.y) * scale),
            (float)((entity->position.z - center.z) * scale)};
        // Draw

        DrawModel(entity->getModel(), pos, entity->radius * scale, WHITE); // 20

        auto points = sim->lines.find((size_t)entity)->second;
        if (points->size() >= 2)
        {
            Vector3 startPos;
            Vector3 endPos;

            for (size_t i = 0; i < points->size() - 1; i += 1)
            {
                startPos = {
                    (float)((points->at(i).x - center.x) * scale),
                    (float)((points->at(i).y - center.y) * scale),
                    (float)((points->at(i).z - center.z) * scale),
                };
                if (i + 1 >= points->size())
                    break;

                try
                {
                    endPos = {
                        (float)((points->at(i + 1).x - center.x) * scale),
                        (float)((points->at(i + 1).y - center.y) * scale),
                        (float)((points->at(i + 1).z - center.z) * scale),
                    };
                }
                catch (const std::exception &e)
                {
                    break;
                }

                Color color = ColorAlpha(entity->color, 0.6 * i / (points->size() - 1));
                DrawLine3D(
                    startPos,
                    endPos,
                    color);
            }
            Color color = ColorAlpha(entity->color, 0.6);

            startPos = {
                (float)((points->back().x - center.x) * scale),
                (float)((points->back().y - center.y) * scale),
                (float)((points->back().z - center.z) * scale),
            };
            endPos = {
                (float)((entity->position.x - center.x) * scale),
                (float)((entity->position.y - center.y) * scale),
                (float)((entity->position.z - center.z) * scale),
            };

            DrawLine3D(startPos, endPos, color);
        }
    }

    EndMode3D();
}
#endif

void Renderer::render(Simulator *sim)
{
    Vector3l center = {
        sim->origin->position.x,
        sim->origin->position.y,
        0};
    ClearBackground(BLACK);
    for (Entity *entity : sim->entities)
    {
        if (entity->texturePath == "")
        {
            DrawCircle(
                round((entity->position.x - center.x) * scale + windowsSize.x / 2),
                round((entity->position.y - center.y) * scale + windowsSize.y / 2),
                entity->radius * scale,
                entity->color);
        }
        else
        {
            auto texture = entity->getTexture();
            float textureScale = (entity->radius * scale * 2.0f) / texture->width;
            DrawTextureEx(
                *texture,
                {
                    (float)round((entity->position.x - center.x) * scale + windowsSize.x / 2) - entity->radius * scale,
                    (float)round((entity->position.y - center.y) * scale + windowsSize.y / 2) - entity->radius * scale,
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
                    round((points->at(i).x - center.x) * scale + windowsSize.x / 2),
                    round((points->at(i).y - center.y) * scale + windowsSize.y / 2),
                    round((points->at(i + 1).x - center.x) * scale + windowsSize.x / 2),
                    round((points->at(i + 1).y - center.y) * scale + windowsSize.y / 2),
                    color);
            }
            Color color = ColorAlpha(entity->color, 0.6);
            DrawLine(
                round((points->back().x - center.x) * scale + windowsSize.x / 2),
                round((points->back().y - center.y) * scale + windowsSize.y / 2),
                round((entity->position.x - center.x) * scale + windowsSize.x / 2),
                round((entity->position.y - center.y) * scale + windowsSize.y / 2),
                color);
        }
    }
}
