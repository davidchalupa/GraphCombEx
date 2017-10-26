#ifndef STATISTICS_H
#define STATISTICS_H
#include <math.h>
#include "graphs.h"

class statistics
{
public:
    statistics();
    static refer min_degree(graph G);
    static refer max_degree(graph G);
    static double average_degree(graph G);
    static double degree_stdev(graph G);
    static refer degree_distribution(graph G, refer *target);
    static unsigned long long triangles(graph G);
    static double mean_clustering_coefficient(graph G);
    static refer girth(graph G);
    static refer diameter(graph G);
    static refer components(graph G);
    static refer long_cycle(graph G, refer *result, bool use_local_search);
};

#endif // STATISTICS_H
