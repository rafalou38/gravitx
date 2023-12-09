#include <string>
#include <iostream>
#include <assert.h>

#include "raylib.h"
#include "utils.hpp"

#include "Entity.hpp"

Entity::Entity(std::string label)
{
    this->label = label;
    this->position_a = {0, 0, 0};
    this->velocity_a = {0, 0, 0};
    this->acceleration_a = {0, 0, 0};

    this->position_b = {0, 0, 0};
    this->velocity_b = {0, 0, 0};
    this->acceleration_b = {0, 0, 0};

    this->position = &this->position_a;
    this->velocity = &this->velocity_a;
    this->acceleration = &this->acceleration_a;

    this->position_freeze = &this->position_a;
    this->velocity_freeze = &this->velocity_a;
    this->acceleration_freeze = &this->acceleration_a;
}

Entity::~Entity()
{
}

void Entity::swap()
{
    if(this->position == &this->position_a)
    {
        this->position_b = this->position_a;
        this->position = &this->position_b;
        this->position_freeze = &this->position_a;
        
        this->velocity_b = this->velocity_a;
        this->velocity = &this->velocity_b;
        this->velocity_freeze = &this->velocity_a;
        
        this->acceleration_b = this->acceleration_a;
        this->acceleration = &this->acceleration_b;
        this->acceleration_freeze = &this->acceleration_a;
    }
    else
    {
        this->position_a = this->position_b;
        this->position = &this->position_a;
        this->position_freeze = &this->position_b;
        
        this->velocity_a = this->velocity_b;
        this->velocity = &this->velocity_a;
        this->velocity_freeze = &this->velocity_b;
        
        this->acceleration_a = this->acceleration_b;
        this->acceleration = &this->acceleration_a;
        this->acceleration_freeze = &this->acceleration_b;
    }
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

Texture2D *Entity::getTexture()
{
    assert(this->texturePath != "");
    if (!textureLoaded)
    {
        this->texture = LoadTexture(texturePath.c_str());
        textureLoaded = true;
    }
    return &this->texture;
}

Model Entity::getModel()
{
    if (!modelLoaded)
    {
        drawMesh = GenMeshSphere(1, 32, 32);
        drawModel = LoadModelFromMesh(drawMesh);
        if(texturePath != ""){
            SetMaterialTexture(&drawModel.materials[0], MATERIAL_MAP_DIFFUSE, *getTexture());
        }
        modelLoaded = true;
    }
    return drawModel;
}

void Entity::setPosition(float x, float y, float z)
{
    this->position->x = x;
    this->position->y = y;
    this->position->z = z;
}
void Entity::setMass(double mass)
{
    this->mass = mass;
}
void Entity::setRadius(float radius)
{
    this->radius = radius;
}
void Entity::setVelocity(float x, float y, float z)
{
    this->velocity->x = x;
    this->velocity->y = y;
    this->velocity->z = z;
}
