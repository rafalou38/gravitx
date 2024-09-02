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

enum LoadPhase
{
    LOAD_WINDOW,
    LOAD_ENTITIES, // SIM
    LOAD_UI,
    LOAD_RENDERER,
    LOAD_SKYBOX,
    DONE,
};

void show_progress(LoadPhase phase, int width, int height)
{
    BeginDrawing();

    ClearBackground(BLACK);
    int text_w = MeasureText("GravitX", 200);
    DrawText("GravitX", width / 2 - text_w / 2, height / 2 - 100, 200, WHITE);

    string sub_text = "Loading ";
    switch (phase)
    {
    case LOAD_WINDOW:
        sub_text += "Window...";
        break;
    case LOAD_ENTITIES:
        sub_text += "Entities...";
        break;
    case LOAD_UI:
        sub_text += "UI...";
        break;
    case LOAD_RENDERER:
        sub_text += "Renderer...";
        break;
    case LOAD_SKYBOX:
        sub_text += "Skybox...";
        break;
    case DONE:
        sub_text += "Done";
        break;
    }
    text_w = MeasureText(sub_text.c_str(), 50);
    DrawText(sub_text.c_str(), width / 2 - text_w / 2, height / 2 + 100 + 50, 50, WHITE);
    EndDrawing();

    cout << sub_text << endl;
    // string tmp;
    // cin >> tmp;
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

    enum LoadPhase current_load_phase = LOAD_WINDOW;

    AppComponents components;
    UI *ui = NULL;
    Renderer *renderer = NULL;
    Simulator *sim = NULL;

    while (!WindowShouldClose())
    {
        if (current_load_phase != DONE)
            show_progress(current_load_phase, width, height);
        if (IsWindowResized())
        {
            width = GetRenderWidth();
            height = GetRenderHeight();
        }

        if (current_load_phase == LOAD_WINDOW)
        {
            current_load_phase = LOAD_ENTITIES;
        }
        else if (current_load_phase == LOAD_ENTITIES)
        {
            sim = new Simulator();
            if (sim == NULL)
            {
                std::cerr << "Failed to create Simulator" << std::endl;
                return 1;
            }
            components.sim = sim;
            sim->LoadSituation("solarSystemAu.xml");
            sim->startExecutors();
            current_load_phase = LOAD_UI;
        }
        else if (current_load_phase == LOAD_UI)
        {
            ui = new UI(&components);
            if (ui == NULL)
            {
                std::cerr << "Failed to create UI" << std::endl;
                return 1;
            }
            components.ui = ui;
            current_load_phase = LOAD_RENDERER;
        }
        else if (current_load_phase == LOAD_RENDERER)
        {
            renderer = new Renderer(components);
            if (renderer == NULL)
            {
                std::cerr << "Failed to create Renderer" << std::endl;
                return 1;
            }
            components.renderer = renderer;
            renderer->setWindowsSize(width, height);
            current_load_phase = LOAD_SKYBOX;
        }
        else if (current_load_phase == LOAD_SKYBOX)
        {
#ifdef SKY_BOX
            if (!skybox_path.empty() and sky_factor != 0 and !renderer->skybox_loaded)
                renderer->initialiseSkybox(skybox_path, sky_factor);
#endif
            current_load_phase = DONE;
        }
        else if (current_load_phase == DONE)
        {
            BeginDrawing();

            // INPUT

            renderer->setWindowsSize(width, height);
            float scroll = GetMouseWheelMove();
            if (scroll != 0 && !ui->isCursorInWindow())
            {
                if (scroll > 0)
                    renderer->setScale(renderer->scale * 1.1);
                else
                    renderer->setScale(renderer->scale * 0.9);
            }
            if (GetKeyPressed() == KEY_O && !ui->isCursorInWindow())
                sim->changeOrigin();

            renderer->update();

#ifdef D3D
            renderer->render3D(sim);
#else
            renderer->render(sim);
#endif
            ui->renderUI();
            DrawText(to_string(GetFPS()).c_str(), 0, height - 10, 6, WHITE);

            EndDrawing();
        }
    }

    if (sim != NULL)
    {
        sim->stopExecutors();
    }
    CloseWindow();
    return 0;
}