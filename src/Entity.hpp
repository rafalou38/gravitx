#ifndef ENTITY_H
#define ENTITY_H
#include <string>

#include "raylib.h"
#include "utils.hpp"

class Entity
{
private:
public:
    std::string label;
    double mass;
    Vector3l position;

    Vector3l velocity;
    Vector3l acceleration;
    Entity *origin;

    Entity(std::string label);
    ~Entity();
    void setPosition(Vector3 position);
    void setPosition(float x, float y);
    void setVelocity(float x, float y);
    void setMass(double mass);
};

#endif