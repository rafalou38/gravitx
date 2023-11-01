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
    UI(Simulator *sim);
    bool isCursorInWindow();
    void renderUI();
    void destroy();
};

#endif