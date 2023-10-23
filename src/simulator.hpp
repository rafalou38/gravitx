#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include "../lib/tinyxml2.h"
#include "Entity.hpp"
#include "consts.hpp"
#include "utils.hpp"
#include <cmath>
#include <unordered_map>

using namespace std;
using namespace tinyxml2;

class Simulator
{
private:
public:
    // Seconds
    float dt = 5.0f;
    std::vector<Entity *> entities;
    unordered_map<size_t, vector<Vector2> *> lines;
    Entity *origin;

    Simulator();
    ~Simulator();
    void Clear();
    void LoadSituation(string name);
    void changeOrigin();
    void computeLines();
    void update();
};

Simulator::Simulator()
{
    lines = unordered_map<size_t, vector<Vector2> *>();
}
Simulator::~Simulator()
{
    this->Clear();
}

void Simulator::changeOrigin(){
    for (size_t i = 0; i < entities.size(); i++)
    {
        if(entities[i] == origin){
            origin = entities[(i+1) % entities.size()];
            break;
        }
    }
}

void Simulator::computeLines()
{
    for (Entity *entity : this->entities)
    {
        const auto r = lines.find((size_t)entity);
        vector<Vector2> *points;
        if (r == lines.end())
        {
            points = new vector<Vector2>();
            lines[(size_t)entity] = points;
        }
        else
        {
            points = r->second;
        }
        Vector2 last = {0, 0};

        if (points->size() > 0)
            last = points->back();

        if(points->size() > MAX_LINES)
            points->erase(points->begin());
        
        
        float distance = pow(abs(entity->position.x - last.x), 2) + pow(abs(entity->position.y - last.y), 2);
        float speed = pow(entity->velocity.x, 2) + pow(entity->velocity.y, 2);

        if (distance / (speed / 10) >= LINE_DISTANCE)
        {
            points->push_back(Vector2{
                (float)entity->position.x,
                (float)entity->position.y});
        }
    }
}

void Simulator::update()
{
    computeLines();
    for (size_t i = 0; i < entities.size(); i++)
    {
        Entity *a = entities[i];
        for (size_t j = i + 1; j < entities.size(); j++)
        {
            if (i == j)
                continue;

            Entity *b = entities[j];
            Vector3l AB = {
                (b->position.x - a->position.x) * pow(10, 3),
                (b->position.y - a->position.y) * pow(10, 3),
                (b->position.z - a->position.z) * pow(10, 3)};

            long double dist2 = (AB.x * AB.x + AB.y * AB.y + AB.z * AB.z); // m^2
            long double dist = sqrtl(dist2);

            long double k = G /* a->mass * b->mass */ / dist2; // m / kg / s^2

            AB.x /= dist;
            AB.y /= dist;
            AB.z /= dist;

            // m/s^2
            a->acceleration.x += AB.x * k * b->mass;
            a->acceleration.y += AB.y * k * b->mass;
            a->acceleration.z += AB.z * k * b->mass;

            b->acceleration.x += AB.x * -k * a->mass;
            b->acceleration.y += AB.y * -k * a->mass;
            b->acceleration.z += AB.z * -k * a->mass;
        }
    }

    for (auto entity : entities)
    {
        entity->velocity.x += entity->acceleration.x * dt;
        entity->velocity.y += entity->acceleration.y * dt;
        entity->velocity.z += entity->acceleration.z * dt;

        entity->position.x += entity->velocity.x * dt * pow(10, -3); // m to km
        entity->position.y += entity->velocity.y * dt * pow(10, -3);
        entity->position.z += entity->velocity.z * dt * pow(10, -3);

        entity->acceleration.x = 0;
        entity->acceleration.y = 0;
        entity->acceleration.z = 0;
    }
}

void Simulator::Clear()
{
    // TODO: clear lines
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
            const char *rawRadius = elem->Attribute("radius");
            char *rawColor = (char *)elem->Attribute("color");

            double x = rawX != NULL ? atof(rawX) : 0;
            double y = rawY != NULL ? atof(rawY) : 0;
            double Vx = rawVx != NULL ? atof(rawVx) : 0;
            double Vy = rawVy != NULL ? atof(rawVy) : 0;
            double mass = rawMass != NULL ? atof(rawMass) : 0;
            double radius = rawRadius != NULL ? atof(rawRadius) : 0;
            if(rawColor == NULL) rawColor = strdup("#ffffff");
            strcat(rawColor, "ff");
            
            

            if (parentEntity != NULL)
            {
                x += parentEntity->position.x;
                y += parentEntity->position.y;
            }

            entity->setPosition(x, y);
            entity->setMass(mass);
            entity->setRadius(radius);
            entity->setVelocity(Vx, Vy);
            entity->setColor(rawColor);

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