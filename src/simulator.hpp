#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include "../lib/tinyxml2.h"
#include "Entity.hpp"
#include "utils.hpp"
#include <cmath>

using namespace std;
using namespace tinyxml2;

class Simulator
{
private:
public:
    // Seconds
    float dt = 10.0f;
    std::vector<Entity *> entities;
    Entity *origin;

    Simulator();
    ~Simulator();
    void Clear();
    void LoadSituation(string name);
    void update();
};

Simulator::Simulator()
{
}
Simulator::~Simulator()
{
    this->Clear();
}

void Simulator::update()
{
    for (size_t i = 0; i < entities.size(); i++)
    {
        Entity* a = entities[i];
        for (size_t j = i + 1; j < entities.size(); j++)
        {
            if(i == j) continue;

            Entity* b = entities[j];
            Vector3l AB = {
                b->position.x - a->position.x,
                b->position.y - a->position.y,
                b->position.z - a->position.z
            };

            long double dist2 = AB.x * AB.x + AB.y * AB.y + AB.z * AB.z;
            long double dist = sqrtl(dist2);

            long double k = G /* a->mass * b->mass */ / dist2;

            AB.x /= dist;
            AB.y /= dist;
            AB.z /= dist;

            a->acceleration.x += AB.x *  k * b->mass;
            a->acceleration.y += AB.y *  k * b->mass;
            a->acceleration.z += AB.z *  k * b->mass;

            b->acceleration.x += AB.x * -k * a->mass;
            b->acceleration.y += AB.y * -k * a->mass;
            b->acceleration.z += AB.z * -k * a->mass;
        }
    }


    for (auto entity : entities)
    {
        // cout << entity->label << " " << entity->velocity.x << " " << entity->velocity.y << " " << entity->position.x << " " << entity->position.y << endl;
        entity->velocity.x += entity->acceleration.x * dt;
        entity->velocity.y += entity->acceleration.y * dt;
        entity->velocity.z += entity->acceleration.z * dt;

        entity->position.x += entity->velocity.x * dt;
        entity->position.y += entity->velocity.y * dt;
        entity->position.z += entity->velocity.z * dt;
    }
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
    function<void(XMLElement *, Entity *)> traverse = [&](XMLElement *root, Entity *parentEntity)
    {
        XMLElement *elem = root->FirstChildElement("Entity");
        while (elem != NULL)
        {
            const char *entityLabel = elem->Attribute("name");
            Entity *entity = new Entity(entityLabel);
            this->entities.push_back(entity);

            if (entityLabel == originName)
                origin = entity;

            const char *rawX = elem->Attribute("x");
            const char *rawY = elem->Attribute("y");
            const char *rawVx = elem->Attribute("Vx");
            const char *rawVy = elem->Attribute("Vy");
            const char *rawMass = elem->Attribute("mass");

            double x = rawX != NULL ? atof(rawX) : 0;
            double y = rawY != NULL ? atof(rawY) : 0;
            double Vx = rawVx != NULL ? atof(rawVx) : 0;
            double Vy = rawVy != NULL ? atof(rawVy) : 0;
            double mass = rawMass != NULL ? atof(rawMass) : 0;

            if (parentEntity != NULL)
            {
                x += parentEntity->position.x;
                y += parentEntity->position.y;
            }

            entity->setPosition(x, y);
            entity->setMass(mass);
            entity->setVelocity(Vx, Vy);

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
    if (err != 0)
    {
        std::cout << "XML Error: " << err << endl;
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