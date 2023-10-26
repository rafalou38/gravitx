#include "raylib.h"
#include <iostream>
#include <vector>
#include "entity.hpp"
#include "simulator.hpp"
#include "renderer.hpp"
#include "UI.hpp"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

    /**
     * SIMULATOR
     */
    Simulator sim = Simulator();
    sim.LoadSituation("earthManyMoon.xml");
    sim.startExecutors();
    // return 0;

    Renderer renderer = Renderer();
    UI ui = UI(&sim);

    int width = 800;
    int height = 450;

    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(width, height, "GravitX");
    renderer.setWindowsSize(width, height);
    // SetWindowPosition(800,800);
    // Vector2 v = GetWindowPosition();
    // cout << v.x << " " << v.y << endl;
    // SetWindowPosition(1520, 2500);
    // SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    // Main game loop
    while (!WindowShouldClose())
    {
        if (IsWindowResized())
        {
            width = GetRenderWidth();
            height = GetRenderHeight();
            renderer.setWindowsSize(width, height);
            ui.setWindowsSize(width, height);
        }

        float scroll = GetMouseWheelMove();
        if (scroll != 0)
        {
            if (scroll > 0)
                renderer.setScale(renderer.scale * 1.1);
            else
                renderer.setScale(renderer.scale * 0.9);
        }
        if (GetKeyPressed() == KEY_O)
            sim.changeOrigin();

        ui.updateUI(&sim, &renderer);
        renderer.update();

        BeginDrawing();
#ifdef D3D
            renderer.render3D(&sim);
#else
            renderer.render(&sim);
#endif
            ui.renderUI();
        EndDrawing();
    }

    CloseWindow();
    sim.stopExecutors();
    return 0;
}