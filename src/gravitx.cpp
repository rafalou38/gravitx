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
    Renderer renderer = Renderer();
    Simulator sim = Simulator();
    UI ui = UI(&sim);
    
    sim.LoadSituation("earthMoon.xml");

    int width = 800;
    int height = 450;

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(width, height, "GravitX");
    renderer.setWindowsSize(width, height);
    // SetWindowPosition(800,800);
    // Vector2 v = GetWindowPosition();
    // cout << v.x << " " << v.y << endl;
    // SetWindowPosition(1520, 2500);
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    // Main game loop
    while (!WindowShouldClose())
    {
        if(IsWindowResized()){
            width = GetRenderWidth();
            height = GetRenderHeight();
            renderer.setWindowsSize(width, height);
            ui.setWindowsSize(width, height);
        }
        for (size_t i = 0; i < 1000; i++)
        {
            sim.update();
        }


        float scroll = GetMouseWheelMove();
        if(scroll != 0){
            if(scroll > 0)
                renderer.setScale(renderer.scale * 1.1);
            else
                renderer.setScale(renderer.scale * 0.9);
        }
        if(GetKeyPressed() == KEY_O) sim.changeOrigin();

        ui.updateUI(&sim, &renderer);

        BeginDrawing();
            renderer.render(&sim);
            ui.renderUI();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}