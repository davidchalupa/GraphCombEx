#ifndef ALGORITHM_IGCOL_H
#define ALGORITHM_IGCOL_H
#include "algorithm.h"

class algorithm_igcol : public algorithm
{
private:
    graph G;
    refer *result;
    refer colors;
    refer max_size;
    long current_class;
    refer fitness_clique;
    refer new_fitness_clique;
    refer max_colors;
    long *permutation_clique;
    long *new_permutation_clique;
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
    bool *is_in_clique;
    long *new_permutation;
    long *new_class_beginnings;
    bool **color_neighbor_matrix;
    long generate_state_inverse_greedy(long permutation[]);
    refer greedy_clique(graph G, long permutation_clique[]);
    void transform_representation(bool ignore_internal_positions=false);
    bool *free_clique_vertices;
    char current_msg[100];
    bool *already_used_in_permutation;
    void initialization(long *permutation);
public:
    algorithm_igcol(graph G, refer max_colors);
    ~algorithm_igcol();
    bool igcol(graph G, bool random_coloring_init, refer *result, refer *clique_size, refer *initial_clique, refer initial_clique_size);
    bool greedy_col(graph G, refer *result, long *permutation);
    unsigned long long get_iterations();    
};

#endif // ALGORITHM_IGCOL_H
