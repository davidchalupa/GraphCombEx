#ifndef ALGORITHM_GREEDYINDSET_H
#define ALGORITHM_GREEDYINDSET_H

#include "algorithm.h"
#include "graphs.h"

class algorithm_greedyindset : public algorithm
{
private:
    graph G;
    bool *have_adjacent_indset_member;
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
    algorithm_greedyindset();
    ~algorithm_greedyindset();
    refer greedy_indset(graph G, refer *result);
};

#endif // ALGORITHM_GREEDYINDSET_H
