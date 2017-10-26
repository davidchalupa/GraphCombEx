#ifndef ALGORITHM_LSCYCLE_H
#define ALGORITHM_LSCYCLE_H

#include "algorithm.h"
#include "graphs.h"

class algorithm_lscycle : public algorithm
{
public:
    algorithm_lscycle();
    ~algorithm_lscycle();
    refer dfs_cycle(graph G, refer *result);
    refer dfs_ls_cycle(graph G, refer *result);
};

#endif // ALGORITHM_LSCYCLE_H
