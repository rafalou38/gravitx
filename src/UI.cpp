
#include <string>
#include <iostream>
#include <filesystem>

#include "../lib/imgui/imgui.h"
#include "../lib/rlImGui/rlImGui.h"
#include "raylib.h"

#include "simulator.hpp"
#include "renderer.hpp"
#include "UI.hpp"
#include "gravitx.hpp"

UI::UI(AppComponents *app)
{
    this->app = app;
    app->sim->dt = exp(dtSliderValue);
    rlImGuiSetup(true);

    std::string path = "./situations";
    for (const auto &entry : std::filesystem::directory_iterator(path))
    {
        if (entry.path().extension() == ".xml")
            situations.push_back(entry.path().filename().string());
    }
}

UI::~UI()
{
    rlImGuiShutdown();
}
bool UI::isCursorInWindow()
{
    Vector2 mousePos = GetMousePosition();
    return mousePos.x >= windowPos.x && mousePos.x <= windowPos.x + windowSize.x && mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + windowSize.y;
}
void UI::renderUI()
{
    rlImGuiBegin(); // starts the ImGui content mode. Make all ImGui calls after this

    // Create a window called "My First Tool", with a menu bar.
    ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Situation"))
        {
            if(ImGui::MenuItem("Recharger", "r")){
                app->sim->LoadSituation(app->sim->situationName);
                app->sim->startExecutors();
            }
            if (ImGui::BeginMenu("Charger nouvelle Situation"))
            {
                for (string situation : situations)
                {
                    if (ImGui::MenuItem(situation.c_str()))
                    {
                        app->sim->LoadSituation(situation);
                        app->sim->startExecutors();
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Simulation"))
        {
            ImGui::MenuItem("Pause", "p");
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    windowPos = ImGui::GetWindowPos();
    windowSize = ImGui::GetWindowSize();

    string label;

    if (ImGui::CollapsingHeader("Simulator", ImGuiTreeNodeFlags_DefaultOpen))
    {
        label = "Dt: " + std::to_string(app->sim->dt) + "s";
        ImGui::Text(label.c_str());
        ImGui::SetNextItemWidth(-1);
        if (ImGui::SliderFloat("Dt", &dtSliderValue, -10, 10))
            app->sim->dt = exp(dtSliderValue);

        label = "Trail size";
        ImGui::Text(label.c_str());
        ImGui::SetNextItemWidth(-1);
        ImGui::SliderFloat("Trail size", &app->sim->maxLines, 0, 2000);
    }

    if (ImGui::CollapsingHeader("Renderer"))
    {
        label = "phi = " + std::to_string(RAD2DEG * app->renderer->cameraPos.phi) + "°";
        ImGui::Text(label.c_str());
        label = "theta = " + std::to_string(RAD2DEG * app->renderer->cameraPos.theta) + "°";
        ImGui::Text(label.c_str());
        label = "scale: 1px = " + std::to_string(1/app->renderer->scale) + "km";
        ImGui::Text(label.c_str());

        ImGui::SameLine();
        if (ImGui::Button("Reset camera"))
        {
            app->renderer->scale = DEFAULT_SCALE;
            app->renderer->resetCamera();
            app->renderer->updateCameraPos();
        }
    }

    if (ImGui::CollapsingHeader("Situation", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // ImGui::SetWindowFontScale(1.5f);
        float textWidth = ImGui::CalcTextSize(app->sim->situationName.c_str()).x;
        float padding = (windowSize.x - textWidth) * 0.5f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding);
        ImGui::Text(app->sim->situationName.c_str());
        // ImGui::SetWindowFontScale(1);

        float height = 64 * min(4.0f, (float)app->sim->entities.size());
        if(ImGui::BeginListBox("##Entity origin list", ImVec2(-1, height))){
            for (auto entity : app->sim->entities)
            {
                if(ImGui::ImageButton((void *)&(entity->getTexture()->id), ImVec2(64, 64))){
                    app->sim->changeOrigin(entity);
                }
                ImGui::SameLine();
                label = entity->label + (app->sim->origin == entity ? " (origin)" : "");
                ImGui::Text(label.c_str());
            }
            ImGui::EndListBox();
        }
    }

    ImGui::End();
    rlImGuiEnd();
}