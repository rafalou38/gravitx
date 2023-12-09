#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"

#include "Entity.hpp"
#include "simulator.hpp"
#include "UI.hpp"
#include "gravitx.hpp"

struct Polar3D
{
    float d;
    float theta;
    float phi;
};

class Renderer
{
private:
#ifdef D3D
    Camera3D camera;
    bool cameraFlipped = false;
    Model skybox;
#endif

    Vector2 windowsSize = Vector2();

    AppComponents components;

    size_t prevCnt = 0;


public:
    TextureCubemap GenTextureCubemap(Shader shader, Texture2D panorama, int size, int format);

    Polar3D cameraPos;
    float scale = DEFAULT_SCALE;

    Renderer(AppComponents components);
    ~Renderer();

    void setScale(float scale) { this->scale = scale; };
    void setWindowsSize(float x, float y) { this->windowsSize = {x, y}; };

    void update();

#ifdef D3D
    void initialiseSkybox();
    void updateCameraPos();
    void render3D(Simulator *sim);
#endif
    void resetCamera();

    void render(Simulator *sim);

    void mainLoop();
};

#endif