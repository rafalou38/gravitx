#include "raylib.h"
#include <iostream>
#include <vector>
#include "Entity.hpp"
#include "simulator.hpp"
#include "renderer.hpp"
#include "UI.hpp"

#include "gravitx.hpp"

void show_help()
{
    cout << "Usage: ./gravitx [options]" << endl;
    cout << "Options:" << endl;
    cout << "\t--skybox <path to .hdr file>" << endl;
    cout << "\t--sky-factor <factor> resolution to load the skybox (5k potato , 13k normal,  14k absolute beast" << endl;

#ifndef SKY_BOX
    cout << "-> SKY_BOX DISABLED in this build" << endl;
#endif
}
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int sky_factor = 0;
    string skybox_path = "";
    for (int i = 0; i < argc; i++)
        if (strcmp(argv[i], "--sky-factor") == 0)
        {
            sky_factor = atoi(argv[i + 1]);
            cout << "Skybox factor: " << sky_factor << endl;
            if (sky_factor != 0 and (sky_factor < 5 or sky_factor > 14))
            {
                std::cerr << "Skybox factor must be in a reasonable range (5k potato, 13k normal,  14k(absolute beast))" << std::endl;
                return 1;
            }
        }
        else if (strcmp(argv[i], "--skybox") == 0)
        {
            skybox_path = argv[i + 1];
            cout << "Skybox path: " << skybox_path << endl;
            if (skybox_path.empty() or !skybox_path.ends_with(".hdr") or !std::filesystem::exists(skybox_path))
            {
                std::cerr << "Skybox path must be a valid .hdr file" << std::endl;
                return 1;
            }
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            show_help();
            return 0;
        }

#ifdef SKY_BOX
    if (!skybox_path.empty() and sky_factor == 0)
    {
        cout << "[!] Skybox factor (--sky-factor) not specified, skybox will not be displayed, try 13" << endl;
        return 1;
    }
    if (skybox_path.empty())
        cout << "[!] Skybox path (--skybox) not specified, skybox will not be displayed" << endl;
#endif

    AppComponents components;

    Simulator sim = Simulator();
    components.sim = &sim;
    sim.LoadSituation("sunEarthMoon.xml");
    sim.startExecutors();

    int width = 800;
    int height = 450;

    SetTraceLogLevel(LOG_WARNING);
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
    if (!skybox_path.empty() and sky_factor != 0)
        renderer.initialiseSkybox(skybox_path, sky_factor);
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
        DrawText(to_string(GetFPS()).c_str(), 0, height - 10, 6, WHITE);
        EndDrawing();
    }

    sim.stopExecutors();
    CloseWindow();
    return 0;
}