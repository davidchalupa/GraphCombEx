#include "algorithm_lscycle.h"

#include <QThread>
#include <time.h>

#include "random_generator.h"

algorithm_lscycle::algorithm_lscycle()
{
}

algorithm_lscycle::~algorithm_lscycle()
{
}

refer algorithm_lscycle::dfs_cycle(graph G, refer *result)
{
    refer longest;
    refer t, trials = 1;
    refer v, j, k, l, m, v_central = 0, queue_size, queue_current, current_distance;
    refer v_trace;
    bool shifting;

    refer *distances = new refer[G->n];
    refer *queue = new refer[G->n];
    refer *parents = new refer[G->n];
    bool *processed = new bool[G->n];
    refer *longest_cycle_vertices = new refer[G->n+1];
    refer *possibilities = new refer[G->n+1];

    random_generator generator;

    // some long cycle
    longest = 0;
    for (t=0;t<trials;t++)
    {
        // srand((unsigned long long) time(NULL));
        for (v_central=0;v_central<G->n;v_central++)
        {
            for (v=0;v<G->n;v++)
            {
                distances[v] = 0;
                processed[v] = 0;
            }
            // depth-first search (queue used as a stack)
            queue_current = 0;
            queue_size = 1;
            queue[0] = v_central;
            while (queue_size)
            {
                v = queue[queue_size-1];
                queue_size--;
                if (processed[v])
                {
                    continue;
                }
                current_distance = distances[v];
                processed[v] = 1;
                for (k=0;k<G->V[v].edgecount;k++)
                {
                    possibilities[k] = k;
                }
                for (k=0;k<G->V[v].edgecount;k++)
                {
                    l = generator.random(0,G->V[v].edgecount-1-k);
                    j = possibilities[l];
                    possibilities[l] = possibilities[G->V[v].edgecount-1-k];
                    if (! processed[G->V[v].sibl[j]])
                    {
                        // in DFS, we do not necessarily consider the shortest distance
                        if (G->V[v].sibl[j] != v_central && distances[G->V[v].sibl[j]] <= current_distance + 1)
                        {
                            // deleting the old occurrence
                            shifting = false;
                            for (l=queue_current;l<queue_current+queue_size;l++)
                            {
                                if (G->V[v].sibl[j] == queue[l])
                                {
                                    shifting = true;
                                }
                                if (shifting && l < queue_current+queue_size-1)
                                {
                                    queue[l] = queue[l+1];
                                }
                            }
                            if (shifting)
                            {
                                queue_size--;
                            }
                            // inserting the new one
                            queue[queue_current+queue_size] = G->V[v].sibl[j];
                            queue_size++;
                            distances[G->V[v].sibl[j]] = current_distance + 1;
                            parents[G->V[v].sibl[j]] = v;
                        }
                    }
                    if (G->V[v].sibl[j] == v_central && current_distance >= 2)
                    {
                        if (longest < current_distance + 1)
                        {
                            longest = current_distance + 1;
                            longest_cycle_vertices[0] = v_central;
                            m = 1;
                            v_trace = v;
                            while (v_trace != v_central)
                            {
                                longest_cycle_vertices[m] = v_trace;
                                v_trace = parents[v_trace];
                                m++;
                            }
                            longest_cycle_vertices[m] = v_central;
                        }
                    }
                }
            }
        }
        // QThread::msleep(500);
    }

    for (m=0;m<longest+1;m++)
    {
        result[m] = longest_cycle_vertices[m];
    }

    delete[](possibilities);
    delete[](distances);
    delete[](queue);
    delete[](parents);
    delete[](processed);
    delete[](longest_cycle_vertices);

    return longest;
}

refer algorithm_lscycle::dfs_ls_cycle(graph G, refer *result)
{
    refer longest;
    refer v, w, v_mid, j, k, l, m;

    refer *longest_cycle_vertices = new refer[G->n+1];

    longest = dfs_cycle(G, longest_cycle_vertices);

    if (0 == longest)
    {
        delete[](longest_cycle_vertices);
        return 0;
    }

    bool *in_cycle = new bool[G->n];
    for (v=0;v<G->n;v++)
    {
        in_cycle[v] = false;
    }
    for (m=0;m<longest+1;m++)
    {
        in_cycle[longest_cycle_vertices[m]] = true;
    }

    bool improvable = false;
    long stagnation_moves_count = 0;
    long picked_move = 0;
    long stag = 0;
    do
    {
        improvable = false;
        stag++;

        // improvement by diversion through a rectangle
        for (m=0;m<longest;m++)
        {
            v = longest_cycle_vertices[m];
            w = longest_cycle_vertices[m+1];

            // searching for rectangles outside the cycle
            for (j=0;j<G->V[v].edgecount;j++)
            {
                for (k=0;k<G->V[w].edgecount;k++)
                {
                    if (! in_cycle[G->V[v].sibl[j]] && ! in_cycle[G->V[w].sibl[k]] && are_adjacent(G->V[v].sibl[j], G->V[w].sibl[k]))
                    {
                        improvable = true;
                        for (l=longest;l>m;l--)
                        {
                            longest_cycle_vertices[l+2] = longest_cycle_vertices[l];
                        }
                        longest_cycle_vertices[m+1] = G->V[v].sibl[j];
                        longest_cycle_vertices[m+2] = G->V[w].sibl[k];
                        in_cycle[G->V[v].sibl[j]] = true;
                        in_cycle[G->V[w].sibl[k]] = true;
                        longest += 2;
                        stag = 0;
                        break;
                    }
                }
                if (improvable)
                {
                    break;
                }
            }
            if (improvable)
            {
                break;
            }
        }

        if (improvable)
        {
            continue;
        }

        // improvement by diversion through a triangle
        for (m=0;m<longest;m++)
        {
            v = longest_cycle_vertices[m];
            w = longest_cycle_vertices[m+1];

            // searching for triangles outside the cycle
            for (j=0;j<G->V[v].edgecount;j++)
            {
                if (! in_cycle[G->V[v].sibl[j]] && are_adjacent(G->V[v].sibl[j], w))
                {
                    improvable = true;
                    for (l=longest;l>m;l--)
                    {
                        longest_cycle_vertices[l+1] = longest_cycle_vertices[l];
                    }
                    longest_cycle_vertices[m+1] = G->V[v].sibl[j];
                    in_cycle[G->V[v].sibl[j]] = true;
                    longest++;
                    stag = 0;
                    break;
                }
            }
            if (improvable)
            {
                break;
            }
        }

        if (improvable)
        {
            continue;
        }

        // stagnation by substitution with an alternative path
        stagnation_moves_count = 0;
        for (m=0;m<longest-1;m++)
        {
            v = longest_cycle_vertices[m];
            v_mid = longest_cycle_vertices[m+1];
            w = longest_cycle_vertices[m+2];

            // searching for the alternative path
            for (j=0;j<G->V[v].edgecount;j++)
            {
                if (! in_cycle[G->V[v].sibl[j]] && are_adjacent(G->V[v].sibl[j], w))
                {
                    stagnation_moves_count++;
                }
            }
        }

        if (0 == stagnation_moves_count)
        {
            break;
        }

        picked_move = generator.random(0, stagnation_moves_count-1);

        for (m=0;m<longest-1;m++)
        {
            v = longest_cycle_vertices[m];
            v_mid = longest_cycle_vertices[m+1];
            w = longest_cycle_vertices[m+2];

            // searching for the alternative path
            for (j=0;j<G->V[v].edgecount;j++)
            {
                if (! in_cycle[G->V[v].sibl[j]] && are_adjacent(G->V[v].sibl[j], w))
                {
                    if (0 == picked_move)
                    {
                        improvable = true;
                        longest_cycle_vertices[m+1] = G->V[v].sibl[j];
                        in_cycle[v_mid] = false;
                        in_cycle[G->V[v].sibl[j]] = true;
                        break;
                    }
                    else
                    {
                        picked_move--;
                    }
                }
            }
        }

        if (stag > 100)
        {
            improvable = false;
        }
    }
    while (improvable);

    for (m=0;m<longest+1;m++)
    {
        result[m] = longest_cycle_vertices[m];
    }

    delete[](in_cycle);
    delete[](longest_cycle_vertices);

    return longest;
}
