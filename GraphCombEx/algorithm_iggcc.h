#ifndef ALGORITHM_IGGCC_H
#define ALGORITHM_IGGCC_H
#include "algorithm.h"

class algorithm_iggcc : public algorithm
{
private:
    graph G;
    refer *result;
    refer colors;
    refer max_size;
    long current_class;
    refer fitness_indset;
    refer new_fitness_indset;
    long *permutation_indset;
    long *new_permutation_indset;
    refer *class_sizes;
    long *clique_sizes;
    long *size_copies;
    bool *changed_sizes;
    long *internal_positions;
    long *class_beginnings;
    long *auxiliary_state;
    long *possibilities;
    long *permutation;
    long *new_ordering;
    vertex_list *vertex_lists;
    bool *have_adjacent_indset_member;
    long *new_permutation;
    long *new_class_beginnings;
    long generate_state_inverse_greedy(long permutation[]);
    refer greedy_indset(graph G, long permutation_indset[]);
    void transform_representation();
    bool *free_indset_vertices;
    char current_msg[100];
    bool *already_used_in_permutation;
    void initialization(long *permutation);
public:
    algorithm_iggcc(graph G);
    ~algorithm_iggcc();
    bool iggcc_ccp(graph G, refer *result, refer *indset_size, refer *initial_indset, refer initial_indset_size);
    bool gcc_ccp(graph G, refer *result, long *permutation);
    unsigned long long get_iterations();    
};

#endif // ALGORITHM_IGGCC_H
