#ifndef ENTITY_H
#define ENTITY_H
#include "raylib.h"
#include <string>

class Entity
{
private:
public:
    std::string label;
    double mass;
    Vector3 position;

    Vector3 velocity;
    Entity *origin;

    Entity(std::string label);
    ~Entity();
    void render();
    void setPosition(Vector3 position);
    void setPosition(float x, float y);
    void setVelocity(float x, float y);
    void setMass(double mass);
};

#endif