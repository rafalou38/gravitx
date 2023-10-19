#include "entity.hpp"
#include <iostream>

Entity::Entity(std::string label)
{
    this->label = label;
}

Entity::~Entity()
{
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