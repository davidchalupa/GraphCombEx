#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "random_generator.h"
#include "graphs.h"

class algorithm
{
protected:
    random_generator generator;
public:
    algorithm();
    virtual ~algorithm();
    virtual refer brelaz_with_heap(graph, refer *) { return 0; }
    virtual refer greedy_indset(graph, refer *) { return 0; }
    virtual refer greedy_clique(graph, refer *) { return 0; }
    virtual bool iggcc_ccp(graph, refer *, refer *, refer *, refer) { return 0; }
    virtual refer greedydom(graph, bool *) { return 0; }
    virtual bool igcol(graph, bool, refer *, refer *, refer *, refer) { return 0; }
};

#endif // ALGORITHM_H
