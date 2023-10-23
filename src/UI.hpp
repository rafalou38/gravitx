
#define RAYGUI_IMPLEMENTATION
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wenum-compare"
#include "../lib/raygui.h"
#pragma GCC diagnostic pop

#include "simulator.hpp"

#define MARGIN 5

class UI
{
private:
    Vector2 mousePosition = {0, 0};
    Vector2 panOffset = {0, 0};
    bool dragWindow = false;
    bool windowBtn = false;
    bool windowCollapsed = false;
    Rectangle windowRect = {0, 0, 200, 200};
    Vector2 bufferSize = {800, 450};
    float sliderValue = 0.5f;
    Simulator *sim;

    void updateWindowRect()
    {
        mousePosition = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !dragWindow)
        {
            if (CheckCollisionPointRec(mousePosition, (Rectangle){windowRect.x, windowRect.y, windowRect.width, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT}))
            {
                dragWindow = true;
                panOffset = {mousePosition.x - windowRect.x, mousePosition.y - windowRect.y};
            }
        }

        if (dragWindow)
        {
            windowRect.x = (mousePosition.x - panOffset.x);
            windowRect.y = (mousePosition.y - panOffset.y);
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                dragWindow = false;
        }

        

        if (windowBtn)
            windowCollapsed = !windowCollapsed;
        if (windowCollapsed)
        {
            // windowRect.width = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT * 4;
            windowRect.height = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
        }
        else
        {
            windowRect.height = 200;
            windowRect.width = 200;
        }
    }

public:
    UI()
    {
    }
    void setWindowsSize(float x, float y) { this->bufferSize = {x, y}; };
    void renderUI(){
        windowBtn = GuiWindowBox(windowRect, "#15#");
        if(windowCollapsed) return;

        float y = windowRect.x + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + MARGIN;
        float x = windowRect.y + MARGIN * 2 + MeasureText("Dt", TEXT_SIZE);
        float prev = sliderValue;
        GuiSlider({x, y, 100, 15}, "Dt", TextFormat("%0.1f", sim->dt), &sliderValue, 0.1f, 10.0f);
        if (prev != sliderValue)
        {
            sim->dt = exp(sliderValue) - 0.99;
        }
    }
    void updateUI(Simulator &sim)
    {
        this->sim = &sim;
        updateWindowRect();
    }
};