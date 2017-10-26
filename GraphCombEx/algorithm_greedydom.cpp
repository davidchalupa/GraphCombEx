#include "algorithm_greedydom.h"

algorithm_greedydom::algorithm_greedydom()
{
}

algorithm_greedydom::~algorithm_greedydom()
{
}

refer algorithm_greedydom::greedydom(graph G, bool *result)
{
    refer j, k, v, max_priority, chosen_vertex, dominating_set_size;
    bool ok;

    priorities = new refer[G->n];
    max_priority_vertices = new refer[G->n];
    dominated = new bool[G->n];

    for (v=0; v<G->n; v++)
    {
        priorities[v] = G->V[v].edgecount + 1;
        dominated[v] = 0;
        result[v] = 0;
    }

    while (1)
    {
        ok = true;
        for (v=0; v<G->n; v++)
        {
            if (! dominated[v])
            {
                ok = false;
                break;
            }
        }
        if (ok)
        {
            break;
        }
        // maximum priority
        // ToDo: we should perhaps use a binary heap to make this operation more efficient
        max_priority = 0;
        for (v=0; v<G->n; v++)
        {
            if (result[v] == 0 && max_priority < priorities[v])
            {
                max_priority = priorities[v];
            }
        }
        // all maximum priority vertices
        max_priority_vertices_count = 0;
        for (v=0; v<G->n; v++)
        {
            if (result[v] == 0 && max_priority == priorities[v])
            {
                max_priority_vertices[max_priority_vertices_count] = v;
                max_priority_vertices_count++;
            }
        }
        // random choice from the maximum priority vertices
        chosen_vertex = max_priority_vertices[generator.random(0, max_priority_vertices_count-1)];
        //chosen_vertex = max_priority_vertices[0];

        // priorities update
        if (! dominated[chosen_vertex])
        {
            priorities[chosen_vertex]--;
        }
        for (j=0; j<G->V[chosen_vertex].edgecount; j++)
        {
            v = G->V[chosen_vertex].sibl[j];
            // if the chosen vertex was not dominated, then its neighbors now have lower priorities
            if (! dominated[chosen_vertex])
            {
                priorities[v]--;
            }
            if (! dominated[v])
            {
                priorities[v]--;
            }
            for (k=0; k<G->V[v].edgecount; k++)
            {
                // if the neighbor was not dominated but now is, then each other neighbor has a lower priority now
                if (! dominated[v])
                {
                    priorities[G->V[v].sibl[k]]--;
                }
            }
        }
        // now dominated vertices
        result[chosen_vertex] = 1;
        dominated[chosen_vertex] = 1;
        for (j=0; j<G->V[chosen_vertex].edgecount; j++)
        {
            v = G->V[chosen_vertex].sibl[j];
            dominated[v] = 1;
        }

    }

    dominating_set_size = 0;
    for (v=0; v<G->n; v++)
    {
        if (result[v])
        {
            dominating_set_size++;
        }
    }

    delete[](dominated);
    delete[](max_priority_vertices);
    delete[](priorities);

    return dominating_set_size;
}
