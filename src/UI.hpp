#ifndef UI_H
#define UI_H

#include <string>
#include <iostream>
#include <filesystem>

#include "../lib/imgui/imgui.h"
#include "../lib/rlImGui/rlImGui.h"
#include "raylib.h"

#include "simulator.hpp"
#include "renderer.hpp"

#define MARGIN 5

class Renderer;
class UI
{
private:
    Simulator *sim;
    Renderer *renderer;
    bool my_tool_active = true;
    float dtSliderValue = -10;

    vector<string> situations;

    ImVec2 windowPos;
    ImVec2 windowSize;

public:
    UI(Simulator *sim)
    {
        this->sim = sim;
        rlImGuiSetup(true);

        std::string path = "./situations";
        for (const auto &entry : std::filesystem::directory_iterator(path))
        {
            if (entry.path().extension() == ".xml")
                situations.push_back(entry.path().filename().string());
        }
    }
    bool isCursorInWindow()
    {
        Vector2 mousePos = GetMousePosition();
        return mousePos.x >= windowPos.x && mousePos.x <= windowPos.x + windowSize.x && mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + windowSize.y;
    }
    void renderUI()
    {
        rlImGuiBegin(); // starts the ImGui content mode. Make all ImGui calls after this


        // Create a window called "My First Tool", with a menu bar.
        ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Situation"))
            {
                ImGui::MenuItem("Recharger ce Situation");
                if (ImGui::BeginMenu("Charger nouvelle Situation"))
                {
                    for (string situation : situations)
                    {
                        if(ImGui::MenuItem(situation.c_str())){
                            sim->LoadSituation(situation);
                            sim->startExecutors();
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Simulation"))
            {
                ImGui::MenuItem("Pause");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        windowPos = ImGui::GetWindowPos();
        windowSize = ImGui::GetWindowSize();

        // ImGui::SetNextItemWidth(-1);
        // ImGui::SliderAngle("phi", &(renderer->cameraPos.phi), 0, 2 * PI);
        // ImGui::SliderAngle("theta", &(renderer->cameraPos.theta), 0, 2 * PI);


        // ImGui::SetNextItemWidth(-1);
        ImGui::SliderFloat("Dt", &dtSliderValue, -10, 10);
        

        // // Edit a color stored as 4 floats
        // ImGui::ColorEdit4("Color", color);

        // // Generate samples and plot them
        // float samples[100];
        // for (int n = 0; n < 100; n++)
        //     samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
        // ImGui::PlotLines("Samples", samples, 100);

        // // Display contents in a scrolling region
        // ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
        // ImGui::BeginChild("Scrolling");
        // for (int n = 0; n < 50; n++)
        //     ImGui::Text("%04d: Some text", n);
        // ImGui::EndChild();
        ImGui::End();
        rlImGuiEnd(); // ends the ImGui content mode. Make all ImGui calls before this
    }

    void destroy()
    {
        rlImGuiShutdown();
    }
};

#endif