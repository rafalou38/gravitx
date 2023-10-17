#include "entity.hpp"
#include <iostream>

Entity::Entity(std::string label)
{
    this->label = label;
}

Entity::~Entity()
{
    std::cout << "entity destroyed;" << std::endl;
}

void Entity::render()
{
    BeginDrawing();
    DrawCircle(
        position.x,
        position.y,
        10,
        WHITE);
}

void Entity::setPosition(float x, float y){
    this->position.x = x;
    this->position.y = y;
}
void Entity::setMass(double mass){
    this->mass = mass;
}
void Entity::setVelocity(float x, float y){
    this->velocity.x = x;
    this->velocity.y = y;
}