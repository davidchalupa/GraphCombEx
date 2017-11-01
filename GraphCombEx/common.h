#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>

// datovy typ pre index vrchola
typedef unsigned int refer;

class vertex_list
{
public:
    bool occ;
    long vertex;
    vertex_list *next;
    vertex_list *prev;
    vertex_list *end;
};

class CommonSettings
{
public:
    static refer runs_greedy_clique;
    static refer runs_brelaz;
    static refer runs_rls_ig_clique;
    static refer runs_greedy_independent_set;
    static refer runs_rls_ig_independent_set;
    static refer runs_greedy_dominating_set;
    static refer runs_dfs;
    static refer runs_dfs_ls;
    static long long crossing_minimization_time_limit;
    static int label_font_size;
    static int vertex_radius;
};

void QuickSort(refer *array, int left, int right);
int BinarySearch(refer *array, refer key, int left, int right);
unsigned long long power(unsigned long long base, unsigned long long exponent);

#endif
