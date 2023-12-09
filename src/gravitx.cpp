#include "raylib.h"
#include <iostream>
#include <vector>
#include "entity.hpp"
#include "simulator.hpp"
#include "renderer.hpp"
#include "UI.hpp"

#include "gravitx.hpp"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    AppComponents components;

    Simulator sim = Simulator();
    components.sim = &sim;
    sim.LoadSituation("earthMoon.xml");
    sim.startExecutors();

    int width = 800;
    int height = 450;

    // SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(width, height, "GravitX");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowState(FLAG_WINDOW_MAXIMIZED);
    SetTargetFPS(60);
    width = GetRenderWidth();
    height = GetRenderHeight();

    BeginDrawing();

    ClearBackground(BLACK);
    int text_w = MeasureText("GravitX", 200);
    DrawText("GravitX", width / 2 - text_w / 2, height / 2 - 100, 200, WHITE);

    text_w = MeasureText("Loading...", 50); 
    DrawText("Loading...", width / 2 - text_w / 2, height / 2 + 100 + 50, 50, WHITE);

    EndDrawing();

    UI ui = UI(&components);
    components.ui = &ui;
    Renderer renderer = Renderer(components);
    components.renderer = &renderer;
    renderer.setWindowsSize(width, height);

    
#ifdef SKY_BOX
    renderer.initialiseSkybox();
#endif
    // Main game loop
    while (!WindowShouldClose())
    {
        if (IsWindowResized())
        {
            width = GetRenderWidth();
            height = GetRenderHeight();
            renderer.setWindowsSize(width, height);
        }

        float scroll = GetMouseWheelMove();
        if (scroll != 0 && !ui.isCursorInWindow())
        {
            if (scroll > 0)
                renderer.setScale(renderer.scale * 1.1);
            else
                renderer.setScale(renderer.scale * 0.9);
        }
        if (GetKeyPressed() == KEY_O && !ui.isCursorInWindow())
            sim.changeOrigin();

        renderer.update();

        BeginDrawing();
        
        // DrawText("50:0", 50 + width / 2, 0 + height / 2, 6, WHITE);
        // DrawText("0:50", 0 + width / 2, 50 + height / 2, 6, WHITE);
        // DrawText("-50:0", -50 + width / 2, 0 + height / 2, 6, WHITE);
        // DrawText("0:-50", 0 + width / 2, -50 + height / 2, 6, WHITE);


#ifdef D3D
        renderer.render3D(&sim);
#else
        renderer.render(&sim);
#endif
        ui.renderUI();
        DrawText(to_string(GetFPS()).c_str(), 0, height-10, 6, WHITE);
        EndDrawing();
    }

    sim.stopExecutors();
    CloseWindow();
    return 0;
}