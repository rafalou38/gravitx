#ifndef ENTITY_H
#define ENTITY_H
#include <string>
#include <iostream>

#include "raylib.h"
#include "utils.hpp"

class Entity
{
private:
public:
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
    void setPosition(float x, float y);
    void setVelocity(float x, float y);
    void setMass(double mass);
    void setRadius(float radius);
    void setColor(char *rawColor);
};

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

void Entity::setColor(char *rawColor){
    rawColor += 1;
    unsigned int colorInt;
    sscanf(rawColor, "%x", &colorInt);
    this->color = GetColor(colorInt);
}
void Entity::setPosition(float x, float y){
    this->position.x = x;
    this->position.y = y;
}
void Entity::setMass(double mass){
    this->mass = mass;
}
void Entity::setRadius(float radius){
    this->radius = radius;
}
void Entity::setVelocity(float x, float y){
    this->velocity.x = x;
    this->velocity.y = y;
}

#endif