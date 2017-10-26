#ifndef PROBLEM_CCP_H
#define PROBLEM_CCP_H
#include "problem.h"
#include "graphs.h"

class problem_ccp : public problem
{
public:
    problem_ccp();
    static unsigned long long count_conflicts(graph G, refer *result);
    static refer find_conflicting_vertices(graph G, refer *result, refer *conflicting_vertices_list, refer *positioning_list);
    static refer find_unlabeled_vertices(graph G, refer *result, refer *unlabeled_vertices_list);
    static void label_unlabeled_vertices_randomly(graph G, refer *result, refer max_label);
    static refer count_labels(graph G, refer *result);
};

#endif // PROBLEM_CCP_H
