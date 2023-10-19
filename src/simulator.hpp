#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include "../lib/tinyxml2.h"
#include "Entity.hpp"

using namespace std;
using namespace tinyxml2;

class Simulator
{
private:
public:
    std::vector<Entity*> entities;
    Entity *origin;
    Simulator();
    ~Simulator();
    void Clear();
    void LoadSituation(string name);
};

Simulator::Simulator()
{
}
Simulator::~Simulator()
{
    this->Clear();
}

void Simulator::Clear()
{
    for (Entity *entity : entities)
    {
        delete entity;
    }
    entities.clear();
}


/**
 * Loads a situation from a file and populates the Simulator with entities.
 *
 * @param name the name of the situation file to load
 *
 * @throws runtime_error if the file is not found or there is an XML error
 */
void Simulator::LoadSituation(string name)
{
    this->Clear();

    string originName;
    function<void(XMLElement*, Entity*)> traverse = [&](XMLElement *root, Entity *parentEntity)
    {
        XMLElement *elem = root->FirstChildElement("Entity");
        while (elem != NULL)
        {
            const char *entityLabel = elem->Attribute("name");
            Entity *entity = new Entity(entityLabel);
            this->entities.push_back(entity);

            if(entityLabel == originName) origin = entity;

            const char *rawX    = elem->Attribute("x");
            const char *rawY    = elem->Attribute("y");
            const char *rawVx   = elem->Attribute("Vx");
            const char *rawVy   = elem->Attribute("Vy");
            const char *rawMass = elem->Attribute("mass");

            double x    = rawX    != NULL ? atof(rawX): 0;
            double y    = rawY    != NULL ? atof(rawY): 0;
            double Vx    = rawVx  != NULL ? atof(rawVx): 0;
            double Vy    = rawVy  != NULL ? atof(rawVy): 0;
            double mass = rawMass != NULL ? atof(rawMass): 0;

            if(parentEntity != NULL){
                x += parentEntity->position.x;
                y += parentEntity->position.y;
            }

            entity->setPosition(x, y);
            entity->setMass(mass);
            entity->setVelocity(Vx,Vy);

            XMLElement *child = elem->FirstChildElement("Entity");
            if (child != NULL)
                traverse(elem, entity);
            
            elem = elem->NextSiblingElement("Entity");
        };
        return;
    };
    string filePath = "situations/" + name;
    // Check if the file exists
    if (!FileExists(filePath.c_str()))
    {
        cerr << "File not found: " << filePath << endl;
        throw runtime_error("Situation not found");
    }
    // Load the file
    XMLDocument doc;
    XMLError err = doc.LoadFile(filePath.c_str());
    if(err != 0){
        std::cout << "XML Error: " << err <<endl;
        throw runtime_error("XML Error");
    }
    XMLElement *root = doc.FirstChildElement();
    originName = root->Attribute("origin");
    traverse(root, NULL);

    std::cout << "Loaded " << this->entities.size() << " entities" << endl;
    for (auto entity : this->entities)
    {
        std::cout << "\t\033[1m" << entity->label << "\033[0m\tx:" << entity->position.x << "\ty:" << entity->position.y << "\tVx:" << entity->velocity.x << "\tVy:" << entity->velocity.y << "\tM:" << entity->mass << endl;
    }
}

#endif