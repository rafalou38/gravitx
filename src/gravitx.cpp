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

    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(width, height, "GravitX");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    UI ui = UI(&sim);
    components.ui = &ui;
    Renderer renderer = Renderer(components);
    components.renderer = &renderer;
    renderer.setWindowsSize(width, height);

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

        // DrawText("0:0", 0 + width / 2, 0 + height / 2, 6, WHITE);
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
        EndDrawing();
    }

    ui.destroy();
    sim.stopExecutors();
    CloseWindow();
    return 0;
}