#include <string>
#include <iostream>
#include <assert.h>

#include "raylib.h"
#include "utils.hpp"

#include "Entity.hpp"

Entity::Entity(std::string label)
{
    this->label = label;
    this->position = {0, 0, 0};
    this->velocity = {0, 0, 0};
    this->acceleration = {0, 0, 0};
}

Entity::~Entity()
{
}

void Entity::setColor(char *rawColor)
{
    rawColor += 1;
    unsigned int colorInt;
    sscanf(rawColor, "%x", &colorInt);
    this->color = GetColor(colorInt);
}
void Entity::setTexture(std::string filePath)
{
    this->texturePath = filePath;
    this->textureLoaded = false;
}

Texture2D Entity::getTexture()
{
    assert(this->texturePath != "");
    if (!textureLoaded)
    {
        this->texture = LoadTexture(texturePath.c_str());
    }
    return this->texture;
}

Model Entity::getModel()
{
    if (!modelLoaded)
    {
        drawMesh = GenMeshSphere(1, 32, 32);
        drawModel = LoadModelFromMesh(drawMesh);
        if(texturePath != ""){
            SetMaterialTexture(&drawModel.materials[0], MATERIAL_MAP_DIFFUSE, getTexture());
        }
        modelLoaded = true;
    }
    return drawModel;
}

void Entity::setPosition(float x, float y)
{
    this->position.x = x;
    this->position.y = y;
}
void Entity::setMass(double mass)
{
    this->mass = mass;
}
void Entity::setRadius(float radius)
{
    this->radius = radius;
}
void Entity::setVelocity(float x, float y)
{
    this->velocity.x = x;
    this->velocity.y = y;
}
