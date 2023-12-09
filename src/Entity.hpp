#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <iostream>

#include "raylib.h"
#include "utils.hpp"

class Entity
{
private:
    Texture2D texture;
    Mesh drawMesh;
    Model drawModel;
    bool textureLoaded = false;
    bool modelLoaded = false;

public:
    std::string texturePath = "";
    std::string label;

    // In kg
    double mass;
    // In km
    float radius;
    Color color;

    // In km
    Vector3l position;
    // In m/s
    Vector3l velocity;
    // In m/s^2
    Vector3l acceleration;

    Entity *origin;
    Entity(std::string label);
    ~Entity();
    void setPosition(Vector3 position);
    void setPosition(float x, float y, float z=0.0f);
    void setVelocity(float x, float y, float z=0.0f);
    void setMass(double mass);
    void setRadius(float radius);
    void setColor(char *rawColor);
    void setTexture(std::string filePath);
    Texture2D *getTexture();
    Model getModel();
};
#endif