#ifndef ALGORITHM_GREEDYCLIQUE_H
#define ALGORITHM_GREEDYCLIQUE_H

#include "algorithm.h"
#include "graphs.h"

class algorithm_greedyclique : public algorithm
{
private:
    graph G;
    bool *is_in_clique;
    // halda
    refer *Q;
    // D[i] is the priority value for i-th element of the heap
    double *D;
    // velkost haldy
    long heapsize;
    refer n;
    void createset();
    refer left(refer i);
    refer right(refer i);
    void heapify(refer i);
    void buildheap();
    refer extractmin();
public:
    algorithm_greedyclique();
    ~algorithm_greedyclique();
    refer greedy_clique(graph G, refer *result);
};

#endif // ALGORITHM_GREEDYCLIQUE_H
