#ifndef UI_H
#define UI_H

#include <string>
#include <iostream>
#include <filesystem>

#include "imgui.h"
#include "rlImGui.h"
#include "raylib.h"

#include "simulator.hpp"
#include "renderer.hpp"
#include "gravitx.hpp"

#define MARGIN 5

class Renderer;
class UI
{
private:
    AppComponents *app;
    bool my_tool_active = true;
    float dtSliderValue = -10;

    vector<string> situations;

    ImVec2 windowPos;
    ImVec2 windowSize;

public:
    UI(AppComponents *app);
    ~UI();
    bool isCursorInWindow();
    void renderUI();
};

#endif