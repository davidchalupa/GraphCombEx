#ifndef ALGORITHM_BRELAZ_H
#define ALGORITHM_BRELAZ_H

#include "algorithm.h"
#include "graphs.h"

class brelaz_heap
{
public:
    brelaz_heap(graph G, double *values);
    ~brelaz_heap();
    // heap itself
    refer *Q;
    // D[i] is the evaluation of the i-th element in the heap
    double *D;
    // H[i] is the position of the i-th vertex in the heap
    refer *H;
    long heapsize;
    refer n;
    void createset();
    refer left(refer i);
    refer right(refer i);
    void heapify(refer i);
    void update(refer i, refer largest);
    void buildheap();
    refer extractmin();
};

class algorithm_brelaz : public algorithm
{
private:
    double *priorities;
    bool **color_neighbor_matrix;
    brelaz_heap *queue;
public:
    algorithm_brelaz();
    ~algorithm_brelaz();
    refer brelaz_with_heap(graph G, refer *result);
};

#endif // ALGORITHM_BRELAZ_H
