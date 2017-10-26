#ifndef ALGORITHM_GREEDYDOM_H
#define ALGORITHM_GREEDYDOM_H

#include "algorithm.h"

class algorithm_greedydom : public algorithm
{
private:
    refer *priorities;
    refer *max_priority_vertices;
    refer max_priority_vertices_count;
    bool *dominated;
public:
    algorithm_greedydom();
    ~algorithm_greedydom();
    refer greedydom(graph G, bool *result);
};

#endif // ALGORITHM_GREEDYDOM_H
