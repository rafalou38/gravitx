#include "raylib.h"
#include <iostream>
#include <vector>

#include "entity.hpp"
#include "simulator.hpp"




//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    Simulator sim = Simulator();
    sim.LoadSituation("earthMoon.xml");

    // int width = 800;
    // int height = 450;

    // SetTraceLogLevel(LOG_WARNING);
    // InitWindow(width, height, "GravitX");
    // SetTargetFPS(60);
    // SetWindowState(FLAG_WINDOW_RESIZABLE);

    // // Main game loop
    // while (!WindowShouldClose())
    // {
    //     if(IsWindowResized()){
    //         width = GetRenderWidth();
    //         height = GetRenderHeight();
    //         std::cout << width << " " << height << std::endl;
    //     }
    //     BeginDrawing();

    //         ClearBackground(BLACK);

    //         earth.render();

    //     EndDrawing();
    // }

    // CloseWindow();
    return 0;
}