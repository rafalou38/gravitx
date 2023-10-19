#include "raylib.h"
#include <iostream>
#include <vector>

#include "entity.hpp"
#include "simulator.hpp"
#include "renderer.hpp"




//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    Renderer renderer = Renderer();
    Simulator sim = Simulator();
    sim.LoadSituation("earthMoon.xml");

    int width = 800;
    int height = 450;

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(width, height, "GravitX");
    renderer.setWindowsSize(width, height);
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    // Main game loop
    while (!WindowShouldClose())
    {
        if(IsWindowResized()){
            width = GetRenderWidth();
            height = GetRenderHeight();
            renderer.setWindowsSize(width, height);
        }
        for (size_t i = 0; i < 1000; i++)
        {
            sim.update();
        }
        
        
        renderer.render(&sim);
    }

    CloseWindow();
    return 0;
}