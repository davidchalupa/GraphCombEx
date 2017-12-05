#include "algorithm_iggcc.h"
#include "mainwindow.h"

algorithm_iggcc::algorithm_iggcc(graph G)
{
    permutation_indset = new long[G->n+1];
    new_permutation_indset = new long[G->n+1];
    class_sizes = new refer[G->n+1];
    clique_sizes = new long[G->n+1];
    size_copies = new long[G->n+1];
    changed_sizes = new bool[G->n+1];
    internal_positions = new long[G->n+1];
    class_beginnings = new long[G->n+1];
    auxiliary_state = new long[G->n+1];
    possibilities = new long[G->n+1];
    permutation = new long[G->n+1];
    new_ordering = new long[G->n+1];
    vertex_lists = new vertex_list[G->n+2];
    have_adjacent_indset_member = new bool[G->n+1];
    new_permutation = new long[G->n+1];
    new_class_beginnings = new long[G->n+1];
    free_indset_vertices = new bool[G->n+1];
    already_used_in_permutation = new bool[G->n+1];
}

algorithm_iggcc::~algorithm_iggcc()
{
    delete[](permutation_indset);
    delete[](new_permutation_indset);
    delete[](class_sizes);
    delete[](clique_sizes);
    delete[](size_copies);
    delete[](changed_sizes);
    delete[](internal_positions);
    delete[](class_beginnings);
    delete[](auxiliary_state);
    delete[](possibilities);
    delete[](permutation);
    delete[](new_ordering);
    delete[](vertex_lists);
    delete[](have_adjacent_indset_member);
    delete[](new_permutation);
    delete[](new_class_beginnings);
    delete[](free_indset_vertices);
    delete[](already_used_in_permutation);
}

refer algorithm_iggcc::greedy_indset(graph G, long permutation_indset[])
{
    refer i,j;
    refer vertex,indset_size;

    for (i=0;i<G->n;i++)
    {
        have_adjacent_indset_member[i] = false;
    }

    indset_size = 0;

    for (i=0;i<G->n;i++)
    {
        vertex = permutation_indset[i];
        // now we look, whether the next guy is not adjacent to some of the indset members
        if (! have_adjacent_indset_member[vertex])
        {
            indset_size++;
            for (j=0;j<G->V[vertex].edgecount;j++)
            {
                have_adjacent_indset_member[G->V[vertex].sibl[j]] = true;
            }
        }
    }

    return indset_size;
}

void algorithm_iggcc::initialization(long *permutation)
{
    refer q,r,s,t,v;
    refer permutation_current;
    bool triangle_found;

    // specific initial permutation
    permutation_current = 0;
    // leaves
    for (q=0;q<G->n;q++)
    {
        possibilities[q] = q;
        already_used_in_permutation[q] = false;
    }
    for (q=0;q<G->n;q++)
    {
        r = generator.random(0,(G->n-1)-q);
        if (1 == G->V[possibilities[r]].edgecount && ! already_used_in_permutation[possibilities[r]] && ! already_used_in_permutation[G->V[possibilities[r]].sibl[0]])
        {
            permutation[permutation_current] = possibilities[r];
            permutation_current++;
            permutation[permutation_current] = G->V[possibilities[r]].sibl[0];
            permutation_current++;
            already_used_in_permutation[possibilities[r]] = true;
            already_used_in_permutation[G->V[possibilities[r]].sibl[0]] = true;
        }
        possibilities[r] = possibilities[(G->n-1)-q];
    }

    // other vertices
    for (q=0;q<G->n;q++)
    {
        possibilities[q] = q;
    }
    for (q=0;q<G->n;q++)
    {
        r = generator.random(0,(G->n-1)-q);
        if (! already_used_in_permutation[possibilities[r]])
        {
            // if there is a triangle to plant, we plant it
            if (1 < G->V[possibilities[r]].edgecount)
            {
                triangle_found = false;
                for (s=0;s<G->V[possibilities[r]].edgecount;s++)
                {
                    v = G->V[possibilities[r]].sibl[s];
                    if (! already_used_in_permutation[v])
                    {
                        for (t=0;t<G->V[v].edgecount;t++)
                        {
                            if (! already_used_in_permutation[G->V[v].sibl[t]] && are_adjacent(G->V[v].sibl[t], possibilities[r]))
                            {
                                triangle_found = true;
                                // planting the triangle
                                permutation[permutation_current] = possibilities[r];
                                permutation_current++;
                                permutation[permutation_current] = v;
                                permutation_current++;
                                permutation[permutation_current] = G->V[v].sibl[t];
                                permutation_current++;
                                already_used_in_permutation[possibilities[r]] = true;
                                already_used_in_permutation[v] = true;
                                already_used_in_permutation[G->V[v].sibl[t]] = true;
                                break;
                            }
                        }
                    }
                    if (triangle_found)
                    {
                        break;
                    }
                }
            }
            if (1 >= G->V[possibilities[r]].edgecount || ! triangle_found)
            {
                permutation[permutation_current] = possibilities[r];
                permutation_current++;
                already_used_in_permutation[possibilities[r]] = true;
            }
        }
        possibilities[r] = possibilities[(G->n-1)-q];
    }

    // random initial permutation
    /*refer q,r;
    for (q=0;q<G->n;q++)
    {
        possibilities[q] = q;
    }
    for (q=0;q<G->n;q++)
    {
        r = generator.random(0,(G->n-1)-q);
        permutation[q] = possibilities[r];
        possibilities[r] = possibilities[(G->n-1)-q];
    }*/
}

bool algorithm_iggcc::iggcc_ccp(graph G, refer *result, refer *indset_size, refer *initial_indset, refer initial_indset_size)
{
    unsigned long long t,w,indset_it,t_stag,t_stag_max;
    long remainder;
    refer q,r,s;
    long aux,rand_handle;
    refer colors_count_old,colors_count;
    refer fitness_indset_old;

    this->G = G;
    this->result = result;
    colors = G->n;
    indset_it = 5;

    initialization(permutation);

    colors_count = generate_state_inverse_greedy(permutation);
    // indset generation
    if (NULL == initial_indset)
    {
        for (q=0;q<G->n;q++)
        {
            possibilities[q] = q;
        }
        for (q=0;q<G->n;q++)
        {
            r = generator.random(0,(G->n-1)-q);
            permutation_indset[q] = possibilities[r];
            possibilities[r] = possibilities[(G->n-1)-q];
        }
        fitness_indset = greedy_indset(G,permutation_indset);
    }
    else
    {
        for (q=0;q<G->n;q++)
        {
            free_indset_vertices[q] = true;
        }
        for (q=0;q<initial_indset_size;q++)
        {
            permutation_indset[q] = initial_indset[q];
            free_indset_vertices[initial_indset[q]] = false;
        }
        r = 0;
        for (q=0;q<G->n;q++)
        {
            if (free_indset_vertices[q])
            {
                possibilities[r] = q;
                r++;
            }
        }
        for (q=0;q<G->n-initial_indset_size;q++)
        {
            r = generator.random(0,(G->n-initial_indset_size-1)-q);
            permutation_indset[q+initial_indset_size] = possibilities[r];
            possibilities[r] = possibilities[(G->n-initial_indset_size-1)-q];
        }
        fitness_indset = greedy_indset(G,permutation_indset);
    }

    // evolution of the permutation
    t = 0;
    t_stag = 0;
    t_stag_max = CommonSettings::max_iter_stag_igccp;
    while (t_stag < t_stag_max && fitness_indset < colors_count)
    {
        colors_count_old = colors_count;
        fitness_indset_old = fitness_indset;

        // greedy clique covering
        colors_count = generate_state_inverse_greedy(permutation);
        for (q=0;q<G->n;q++)
        {
            permutation[q] = auxiliary_state[q];
        }

        // determining the sizes of the classes
        remainder = G->n;
        for (q=0;q<=colors_count-1;q++)
        {
            class_sizes[q] = class_beginnings[q+1] - class_beginnings[q];
            remainder -= class_sizes[q];
        }
        class_sizes[colors_count] = remainder;

        if (t % (1 + 20000000 / G->n) == 0)
        {
            sprintf(current_msg,"%u - %u",fitness_indset,colors_count);
            logMessageIGRLSCliqueCovering(current_msg);
        }

        // reordering the classes
        for (q=0;q<G->n;q++)
        {
            new_ordering[q] = 0;
        }

        rand_handle = generator.random(5,12);
        if (t > 0 && rand_handle < 5)
        {
            // initializing the vertex lists
            for (q=1;q<=colors_count;q++)
            {
                vertex_lists[q].occ = false;
                vertex_lists[q].next = NULL;
                vertex_lists[q].prev = NULL;
                vertex_lists[q].end = &vertex_lists[q];
            }
            max_size = -1;
            for (q=1;q<=colors_count;q++)
            {
                if (max_size < class_sizes[q])
                {
                    max_size = class_sizes[q];
                }
                // if it is the first in a list
                if (! vertex_lists[class_sizes[q]].occ)
                {
                    vertex_lists[class_sizes[q]].occ = true;
                    vertex_lists[class_sizes[q]].vertex = q;
                }
                else
                {
                    // adding to the end of the equal-size list
                    vertex_list *current_vertex;
                    current_vertex = vertex_lists[class_sizes[q]].end;
                    current_vertex->next = new vertex_list();
                    current_vertex->next->vertex = q;
                    current_vertex->next->next = NULL;
                    current_vertex->next->prev = current_vertex;
                    vertex_lists[class_sizes[q]].end = current_vertex->next;
                }
            }
            // finding the new ordering and deleting all the auxiliary stuff
            q = 1;
            for (r=max_size;r>=1;r--)
            {
                if (! vertex_lists[r].occ)
                {
                    continue;
                }
                vertex_list *current_vertex;
                current_vertex = vertex_lists[r].end;
                while (current_vertex->prev != NULL)
                {
                    new_ordering[q] = current_vertex->vertex;
                    q++;
                    current_vertex = current_vertex->prev;
                    delete(current_vertex->next);
                }
                new_ordering[q] = current_vertex->vertex;
                q++;
                current_vertex->occ = false;
                current_vertex->end = current_vertex;
            }
        }
        else if (rand_handle < 10)
        {
            // reverse ordering
            for (q=1;q<=colors_count;q++)
            {
                new_ordering[q] = colors_count-q+1;
            }
        }
        else if (rand_handle < 13)
        {
            // random shuffle
            for (q=1;q<=colors_count;q++)
            {
                possibilities[q] = q;
            }
            for (q=1;q<=colors_count;q++)
            {
                r = generator.random(1,colors_count+1-q);
                new_ordering[q] = possibilities[r];
                possibilities[r] = possibilities[colors_count+1-q];
            }
        }
        else
        {
            // random first, other shifted
            r = generator.random(1,colors_count);
            new_ordering[1] = r;
            for (q=2;q<=r;q++)
            {
                new_ordering[q] = q-1;
            }
            for (q=r+1;q<=colors_count;q++)
            {
                new_ordering[q] = q;
            }
        }

        // applying the new ordering
        s = 0;
        for (q=1;q<=colors_count;q++)
        {
            current_class = new_ordering[q];
            for (r=0;r<class_sizes[current_class];r++)
            {
                new_permutation[s+r] = permutation[class_beginnings[current_class]+r];
            }
            new_class_beginnings[current_class] = s;
            s += class_sizes[current_class];
        }

        // pure IG-GCC
        for (q=0;q<=G->n;q++)
        {
            permutation[q] = new_permutation[q];
        }

        // updating the independent set bound
        for (w=0;w<indset_it;w++)
        {
            r = generator.random(1,G->n-1);
            aux = permutation_indset[r];
            for (s=0;s<r;s++)
            {
                new_permutation_indset[s+1] = permutation_indset[s];
            }
            for (s=r+1;s<G->n;s++)
            {
                new_permutation_indset[s] = permutation_indset[s];
            }
            new_permutation_indset[0] = aux;

            new_fitness_indset = greedy_indset(G,new_permutation_indset);
            if (new_fitness_indset >= fitness_indset)
            {
                for (s=0;s<G->n;s++)
                {
                    permutation_indset[s] = new_permutation_indset[s];
                }
                if (fitness_indset < new_fitness_indset)
                {
                    fitness_indset = new_fitness_indset;
                }
            }
        }

        // stagnation monitoring
        if (colors_count_old > colors_count || fitness_indset_old > fitness_indset)
        {
            t_stag = 0;
        }
        else
        {
            t_stag++;
        }

        t++;
    }

    //printf(" [iterations: %llu][independent set: %ld]\n",t,fitness_indset);
    *indset_size = fitness_indset;

    return (fitness_indset == colors_count);
}

bool algorithm_iggcc::gcc_ccp(graph G, refer *result, long *permutation)
{
    this->G = G;
    this->result = result;
    colors = G->n;

    generate_state_inverse_greedy(permutation);

    return true;
}

unsigned long long algorithm_iggcc::get_iterations()
{
    return 0;
}

long algorithm_iggcc::generate_state_inverse_greedy(long permutation[])
{
    long i,q,vertex,color;
    refer j,l;
    refer current_colors;

    for (j=0;j<G->n;j++)
    {
        clique_sizes[j] = 0;
        size_copies[j] = 0;
        changed_sizes[j] = false;
        result[j] = 0;
        internal_positions[j] = 0;
    }

    current_colors = 0;
    for (j=0;j<G->n;j++)
    {
        vertex = permutation[j];

        // we have the vertex, we find a color for it
        color = 0;
        for (l=0;l<G->V[vertex].edgecount;l++)
        {
            i = G->V[vertex].sibl[l];
            q = result[i];
            if (q > 0)
            {
                if (! changed_sizes[q])
                {
                    size_copies[q] = clique_sizes[q];
                    changed_sizes[q] = true;
                }
                clique_sizes[q]--;
                if (clique_sizes[q] == 0)
                {
                    if (color == 0 || color > q)
                    {
                        color = q;
                    }
                }
            }
        }
        if (color == 0 && current_colors < colors)
        {
            current_colors++;
            color = current_colors;
        }
        // restoration of the sizes
        for (l=0;l<G->V[vertex].edgecount;l++)
        {
            i = G->V[vertex].sibl[l];
            q = result[i];
            if (q > 0 && changed_sizes[q])
            {
                clique_sizes[q] = size_copies[q];
                changed_sizes[q] = false;
            }
        }
        // coloring the vertex
        result[vertex] = color;
        internal_positions[vertex] = clique_sizes[color];
        clique_sizes[color]++;
    }

    transform_representation();

    return current_colors;
}

void algorithm_iggcc::transform_representation()
{
    refer i;
    long sum;

    for (i=0;i<=colors;i++)
    {
        class_sizes[i] = 0;
    }
    for (i=0;i<G->n;i++)
    {
        auxiliary_state[i] = 0;
    }

    // counting the colors and finding the beginnings
    for (i=0;i<G->n;i++)
    {
        class_sizes[result[i]]++;
    }
    sum = 0;
    for (i=0;i<=colors;i++)
    {
        class_beginnings[i] = sum;
        sum += class_sizes[i];
    }
    for (i=0;i<=colors;i++)
    {
        class_sizes[i] = 0;
    }

    // putting the values into the auxiliary state
    for (i=0;i<G->n;i++)
    {
        if (internal_positions[i] >= 0)
        {
            auxiliary_state[class_beginnings[result[i]]+internal_positions[i]] = i;
        }
        else
        {
            auxiliary_state[class_beginnings[result[i]]+class_sizes[result[i]]] = i;
        }
        class_sizes[result[i]]++;
    }
}

