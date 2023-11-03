#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <barrier>
#include <unistd.h>

#include "../lib/tinyxml2.h"

#include "consts.hpp"
#include "Entity.hpp"
#include "utils.hpp"

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
    string situationName;

    float maxLines = MAX_LINES;
    float lineDistance = LINE_DISTANCE;
    // Seconds
    float dt = 1.0f;
    long double time = 0;
    std::vector<Entity *> entities;
    unordered_map<size_t, vector<Vector3> *> lines;
    Entity *origin;


    Simulator();
    ~Simulator();
    void Clear();
    void LoadSituation(string name);
    void changeOrigin();
    void changeOrigin(Entity *entity);
    void startExecutors();
    void stopExecutors();
};

#endif