#include "algorithm_igcol.h"
#include "mainwindow.h"

algorithm_igcol::algorithm_igcol(graph G, refer max_colors)
{
    refer i;

    permutation_clique = new long[G->n+1];
    new_permutation_clique = new long[G->n+1];
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
    is_in_clique = new bool[G->n+1];
    new_permutation = new long[G->n+1];
    new_class_beginnings = new long[G->n+1];
    free_clique_vertices = new bool[G->n+1];
    already_used_in_permutation = new bool[G->n+1];

    this->max_colors = max_colors;
    color_neighbor_matrix = new bool*[G->n+1];
    for (i=1;i<=max_colors;i++)
    {
        color_neighbor_matrix[i] = new bool[G->n];
    }
}

algorithm_igcol::~algorithm_igcol()
{
    refer i;

    delete[](permutation_clique);
    delete[](new_permutation_clique);
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
    delete[](is_in_clique);
    delete[](new_permutation);
    delete[](new_class_beginnings);
    delete[](free_clique_vertices);
    delete[](already_used_in_permutation);

    for (i=1;i<=max_colors;i++)
    {
        delete[](color_neighbor_matrix[i]);
    }
    delete[](color_neighbor_matrix);
}

refer algorithm_igcol::greedy_clique(graph G, long permutation_clique[])
{
    refer i,j;
    refer vertex;
    refer clique_size;
    refer neighbors_in_clique;

    this->G = G;

    for (i=0;i<G->n;i++)
    {
        is_in_clique[i] = false;
    }

    clique_size = 0;

    for (i=0;i<G->n;i++)
    {
        vertex = permutation_clique[i];
        // now we look, whether the next guy is adjacent to all of the clique members
        neighbors_in_clique = 0;
        for (j=0;j<G->V[vertex].edgecount;j++)
        {
            if (is_in_clique[G->V[vertex].sibl[j]])
            {
                neighbors_in_clique++;
            }
        }
        if (neighbors_in_clique == clique_size)
        {
            is_in_clique[vertex] = 1;
            clique_size++;
        }
    }

    return clique_size;
}

void algorithm_igcol::initialization(long *permutation)
{
    // random initial permutation
    refer q,r;
    for (q=0;q<G->n;q++)
    {
        possibilities[q] = q;
    }
    for (q=0;q<G->n;q++)
    {
        r = generator.random(0,(G->n-1)-q);
        permutation[q] = possibilities[r];
        possibilities[r] = possibilities[(G->n-1)-q];
    }
}

bool algorithm_igcol::igcol(graph G, bool random_coloring_init, refer *result, refer *clique_size, refer *initial_clique, refer initial_clique_size)
{
    unsigned long long t,w,clique_it,t_stag,t_stag_max;
    long remainder;
    refer q,r,s;
    long aux,rand_handle;
    refer colors_count_old,colors_count;
    refer fitness_clique_old;

    this->G = G;
    this->result = result;
    colors = G->n;
    clique_it = 5;

    // if we begin with a random permutation, we do the generation and the mapping
    if (random_coloring_init)
    {
        initialization(permutation);
    }
    // if we start with a predefined solution, we only do the representation transformation
    else
    {
        // transformation with ignoring of the internal positions (we do not know them yet)
        transform_representation(true);
        for (q=0;q<G->n;q++)
        {
            permutation[q] = auxiliary_state[q];
        }
    }

    colors_count = generate_state_inverse_greedy(permutation);

    // clique generation
    if (NULL == initial_clique)
    {
        for (q=0;q<G->n;q++)
        {
            possibilities[q] = q;
        }
        for (q=0;q<G->n;q++)
        {
            r = generator.random(0,(G->n-1)-q);
            permutation_clique[q] = possibilities[r];
            possibilities[r] = possibilities[(G->n-1)-q];
        }
        fitness_clique = greedy_clique(G,permutation_clique);
    }
    else
    {
        for (q=0;q<G->n;q++)
        {
            free_clique_vertices[q] = true;
        }
        for (q=0;q<initial_clique_size;q++)
        {
            permutation_clique[q] = initial_clique[q];
            free_clique_vertices[initial_clique[q]] = false;
        }
        r = 0;
        for (q=0;q<G->n;q++)
        {
            if (free_clique_vertices[q])
            {
                possibilities[r] = q;
                r++;
            }
        }
        for (q=0;q<G->n-initial_clique_size;q++)
        {
            r = generator.random(0,(G->n-initial_clique_size-1)-q);
            permutation_clique[q+initial_clique_size] = possibilities[r];
            possibilities[r] = possibilities[(G->n-initial_clique_size-1)-q];
        }
        fitness_clique = greedy_clique(G,permutation_clique);
    }

    // evolution of the permutation
    t = 0;
    t_stag = 0;
    t_stag_max = CommonSettings::max_iter_stag_igcol;
    while (t_stag < t_stag_max && fitness_clique < colors_count)
    {
        colors_count_old = colors_count;
        fitness_clique_old = fitness_clique;

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
            sprintf(current_msg,"%u - %u",fitness_clique,colors_count);
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
        for (w=0;w<clique_it;w++)
        {
            r = generator.random(1,G->n-1);
            aux = permutation_clique[r];
            for (s=0;s<r;s++)
            {
                new_permutation_clique[s+1] = permutation_clique[s];
            }
            for (s=r+1;s<G->n;s++)
            {
                new_permutation_clique[s] = permutation_clique[s];
            }
            new_permutation_clique[0] = aux;

            new_fitness_clique = greedy_clique(G,new_permutation_clique);
            if (new_fitness_clique >= fitness_clique)
            {
                for (s=0;s<G->n;s++)
                {
                    permutation_clique[s] = new_permutation_clique[s];
                }
                if (fitness_clique < new_fitness_clique)
                {
                    fitness_clique = new_fitness_clique;
                }
            }
        }

        // stagnation monitoring
        if (colors_count_old > colors_count || fitness_clique_old > fitness_clique)
        {
            t_stag = 0;
        }
        else
        {
            t_stag++;
        }

        t++;
    }

    //printf(" [iterations: %llu][independent set: %ld]\n",t,fitness_clique);
    *clique_size = fitness_clique;

    return (fitness_clique == colors_count);
}

bool algorithm_igcol::greedy_col(graph G, refer *result, long *permutation)
{
    this->G = G;
    this->result = result;
    colors = G->n;

    generate_state_inverse_greedy(permutation);

    return true;
}

unsigned long long algorithm_igcol::get_iterations()
{
    return 0;
}

long algorithm_igcol::generate_state_inverse_greedy(long permutation[])
{
    refer i,vertex,color;
    refer j,c;
    refer current_colors;

    for (j=0;j<G->n;j++)
    {
        clique_sizes[j] = 0;
        size_copies[j] = 0;
        changed_sizes[j] = false;
        result[j] = 0;
        internal_positions[j] = 0;
    }

    for (c=1;c<=max_colors;c++)
    {
        for (i=0;i<G->n;i++)
        {
            color_neighbor_matrix[c][i] = 0;
        }
    }

    current_colors = 0;
    for (j=0;j<G->n;j++)
    {
        vertex = permutation[j];

        // we have the vertex, we find a color for it
        color = 0;
        for (c=1;c<=G->V[vertex].edgecount+1;c++)
        {
            if (c > current_colors || color_neighbor_matrix[c][vertex] == 0)
            {
                color = c;
                if (color > current_colors)
                {
                    current_colors = color;
                }
                break;
            }
        }
        // coloring the vertex
        result[vertex] = color;
        internal_positions[vertex] = clique_sizes[color];
        clique_sizes[color]++;
        // all the neighbors now do have a colored neighbor
        for (i=0;i<G->V[vertex].edgecount;i++)
        {
            color_neighbor_matrix[color][G->V[vertex].sibl[i]]++;
        }
    }

    transform_representation();

    return current_colors;
}

void algorithm_igcol::transform_representation(bool ignore_internal_positions)
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
        if (! ignore_internal_positions && internal_positions[i] >= 0)
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

