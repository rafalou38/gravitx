#ifndef GRAVITX_H
#define GRAVITX_H

#include "simulator.hpp"
#include "renderer.hpp"
#include "UI.hpp"

class UI;
class Renderer;
class Simulator;
struct AppComponents {
    UI *ui;
    Renderer *renderer;
    Simulator *sim;
};


#endif