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
#include <thread>
#include <barrier>
#include <unistd.h>

using namespace std;
using namespace tinyxml2;

struct Pair
{
    Entity *a;
    Entity *b;
};

class Simulator
{
private:
    unordered_map<size_t, Vector3> previousVelocity;

    std::vector<Pair> pairs;
    size_t executorCount = 0;
    std::atomic<bool> executorShouldStop = false;
    std::thread threads[MAX_THREADS + 1];
    std::unique_ptr<std::barrier<>> sync_point1;
    // std::unique_ptr<std::barrier<>> sync_point2;

    void executor(size_t executor_id, size_t pairsRangeStart, size_t pairsRangeEnd, size_t entityRangeStart, size_t entityRangeEnd);
    void computeInteraction(Entity *a, Entity *b);
    void computeLines(Entity *entity);
public:
    float maxLines = MAX_LINES;
    float lineDistance = LINE_DISTANCE;
    // Seconds
    float dt = 5.0f;
    long double time = 0;
    std::vector<Entity *> entities;
    unordered_map<size_t, vector<Vector3> *> lines;
    Entity *origin;


    Simulator();
    ~Simulator();
    void Clear();
    void LoadSituation(string name);
    void changeOrigin();
    void startExecutors();
    void stopExecutors();
};

Simulator::Simulator()
{
    lines = unordered_map<size_t, vector<Vector3> *>();
    previousVelocity = unordered_map<size_t, Vector3>();
}
Simulator::~Simulator()
{
    this->Clear();
}

void Simulator::changeOrigin()
{
    for (size_t i = 0; i < entities.size(); i++)
    {
        if (entities[i] == origin)
        {
            origin = entities[(i + 1) % entities.size()];
            break;
        }
    }
}

void Simulator::computeLines(Entity *entity)
{
    const auto r = lines.find((size_t)entity);
    vector<Vector3> *points;
    if (r == lines.end())
    {
        points = new vector<Vector3>();
        lines[(size_t)entity] = points;
    }
    else
    {
        points = r->second;
    }
    Vector3 lastPosition = {0, 0};
    Vector3 lastVelocity = {0, 1};

    if (points->size() > 0)
    {
        lastPosition = points->back();
        lastVelocity = previousVelocity[(size_t)entity];
    }

    while (points->size() > maxLines)
        points->erase(points->begin());

    float dot = entity->velocity.x * lastVelocity.x + entity->velocity.y * lastVelocity.y;
    float det = entity->velocity.x * lastVelocity.y - entity->velocity.y * lastVelocity.x;
    float angle = atan2(det, dot);

    if (abs(angle) >= LINE_ANGLE || abs(entity->position.x - lastPosition.x) > lineDistance || abs(entity->position.y - lastPosition.y) > lineDistance)
    {
        points->push_back(Vector3{
            (float)entity->position.x,
            (float)entity->position.y});
        previousVelocity[(size_t)entity] = Vector3{
            (float)entity->velocity.x,
            (float)entity->velocity.y};
    }
}

void Simulator::stopExecutors()
{
    this->executorShouldStop.store(true);
    for (size_t i = 0; i < executorCount; i++)
    {
        threads[i].join();
    }
}
void Simulator::startExecutors()
{
    for (size_t i = 0; i < this->entities.size(); i++)
    {
        Entity *a = entities[i];
        for (size_t j = i + 1; j < this->entities.size(); j++)
        {
            Entity *b = entities[j];
            pairs.push_back(Pair{a, b});
        }
    }

    const size_t processor_count = std::thread::hardware_concurrency();
    executorCount = max(
        (size_t)1,
        min(
            (size_t)MAX_THREADS,
            min(processor_count - 1, (size_t)floor(pairs.size() / 50))));

    cout << "SIMULATOR: using \033[1m" << executorCount << "\033[0m/" << processor_count << " threads" << endl;

    sync_point1 = std::make_unique<std::barrier<>>(executorCount);
    // sync_point2 = std::make_unique<std::barrier<>>(executorCount);

    size_t pairsCursor = 0;
    size_t pairsChunk = floor((this->pairs.size()) / executorCount);
    size_t pairsRest = this->pairs.size() % executorCount;

    size_t entityCursor = 0;
    size_t entityChunk = floor((this->entities.size()) / executorCount);
    size_t entityRest = this->entities.size() % executorCount;
    cout << "entities: " << this->entities.size() << " pairs: " << this->pairs.size() << endl;

    for (size_t i = 0; i < executorCount; i++)
    {
        size_t pairsRangeStart = pairsCursor;
        pairsCursor += pairsChunk;
        size_t pairsRangeEnd = pairsCursor - 1;
        if (pairsRest > 0)
        {
            pairsRangeEnd++;
            pairsCursor++;
            pairsRest--;
        }

        size_t entityRangeStart = entityCursor;
        entityCursor += entityChunk;
        size_t entityRangeEnd = entityCursor - 1;
        if (entityRest > 0)
        {
            entityRangeEnd++;
            entityCursor++;
            entityRest--;
        }

        cout << "SIMULATOR: starting thread " << i << " with pair range: " << pairsRangeStart << " -> " << pairsRangeEnd << " and entity range: " << entityRangeStart << " -> " << entityRangeEnd << endl;
        threads[i] = thread(&Simulator::executor, this, i, pairsRangeStart, pairsRangeEnd, entityRangeStart, entityRangeEnd);
    }
}
void Simulator::executor(size_t executor_id, size_t pairsRangeStart, size_t pairsRangeEnd, size_t entityRangeStart, size_t entityRangeEnd)
{
    while (1)
    {
        for (size_t i = pairsRangeStart; i <= pairsRangeEnd; i++)
        {
            Entity *a = pairs[i].a;
            Entity *b = pairs[i].b;
            computeInteraction(a, b);
        }

        // cout << "SIMULATOR: executor " << executor_id << " waiting" << endl;
        sync_point1->arrive_and_wait();
        // cout << "SIMULATOR: executor " << executor_id << " continue" << endl;

        for (size_t i = entityRangeStart; i <= entityRangeEnd; i++)
        {
            Entity *entity = entities[i];
            computeLines(entity);
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

        if (executorShouldStop.load(std::memory_order_relaxed))
        {
            cout << "SIMULATOR: executor " << executor_id << " stopped" << endl;
            return;
        }
    }
}
void Simulator::computeInteraction(Entity *a, Entity *b)
{
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

void Simulator::Clear()
{
    this->stopExecutors();
    for(auto line : lines){
        delete line.second;
    }
    lines.clear();
    for (Entity *entity : entities)
    {
        delete entity;
    }
    entities.clear();

    pairs.clear();

    executorCount = 0;
    executorShouldStop = false;
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
            const char *textureFile = elem->Attribute("texture");
            char *rawColor = (char *)elem->Attribute("color");

            double x = rawX != NULL ? atof(rawX) : 0;
            double y = rawY != NULL ? atof(rawY) : 0;
            double Vx = rawVx != NULL ? atof(rawVx) : 0;
            double Vy = rawVy != NULL ? atof(rawVy) : 0;
            double mass = rawMass != NULL ? atof(rawMass) : 0;
            double radius = rawRadius != NULL ? atof(rawRadius) : 0;

#ifdef TEXTURES
            if (textureFile != NULL)
            {
                string filePath = "situations/textures/" + string(textureFile);
                if (!FileExists(filePath.c_str()))
                {
                    cerr << "Texture not found: " << filePath << endl;
                    throw runtime_error("Texture not found");
                }
                entity->setTexture(filePath);
            }
#endif

            if (rawColor == NULL)
                rawColor = strdup("ffffff");
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
        // Reconfigure lines 
        computeLines(entity);
        std::cout << "\t\033[1m" << entity->label << "\033[0m\tx:" << entity->position.x << "\ty:" << entity->position.y << "\tVx:" << entity->velocity.x << "\tVy:" << entity->velocity.y << "\tM:" << entity->mass << endl;
    }
}

#endif