
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
    float sliderValue = 0.0f;
    Simulator *sim;
    Renderer *renderer;

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
            // Reset by renderUI
            // windowRect.height = 200;
            // windowRect.width = 200;
        }
    }

public:
    UI(Simulator *sim)
    {
        this->sim = sim;
        sim->dt = exp(sliderValue);
    }
    void setWindowsSize(float x, float y) { this->bufferSize = {x, y}; };
    void renderUI(){
        int fps = GetFPS();
        DrawText(TextFormat("%d", fps), 0, bufferSize.y - 16, 16, WHITE);

        windowBtn = GuiWindowBox(windowRect, "#15#");
        if(windowCollapsed) return;

        float fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
        #define SLIDER_HEIGHT  15
        float y = windowRect.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + MARGIN;
        float x = windowRect.x + MARGIN * 2 + MeasureText("Dt", fontSize/2);
        float prev = sliderValue;
        GuiSlider({x, y, 200 - ((x + 8 * MARGIN) - windowRect.x), SLIDER_HEIGHT}, "Dt", TextFormat("%0.001f", sim->dt), &sliderValue, -10.0f, 10.0f);
        if (prev != sliderValue)
        {
            sim->dt = exp(sliderValue);
        }
        x = windowRect.x + MARGIN;

        y += SLIDER_HEIGHT + MARGIN * 2;        DrawText(TextFormat("Trail size: %0.0f", sim->maxLines), x, y, fontSize, BLACK);
        y += fontSize + MARGIN;        GuiSlider({x, y, 200 - 2*MARGIN, SLIDER_HEIGHT}, "", "", &sim->maxLines, 10.0f, 2000.0f);

        // y += SLIDER_HEIGHT + MARGIN * 2;        DrawText(TextFormat("Trail distance: %0.0f", sim->lineDistance), x, y, fontSize, BLACK);
        // y += fontSize + MARGIN;        GuiSlider({x, y, 200 - 2*MARGIN, SLIDER_HEIGHT}, "", "", &sim->lineDistance, 1000.0f, 100000.0f);

        y += SLIDER_HEIGHT + MARGIN * 2;
        DrawText(TextFormat("Scale: %0.0f Km = 1px", 1/renderer->scale), x, y, fontSize, BLACK);
        y+= fontSize + MARGIN;

        auto d = minutesToDetailedTime(sim->time);
        DrawText(TextFormat("Time: %dy %dd %dh %dm",d.years, d.days, d.hours, d.minutes), x, y, fontSize, BLACK);
        y+= fontSize + MARGIN;

        windowRect.height = y - windowRect.y;
    }
    void updateUI(Simulator *sim, Renderer *renderer)
    {
        this->sim = sim;
        this->renderer = renderer;
        updateWindowRect();
    }
};