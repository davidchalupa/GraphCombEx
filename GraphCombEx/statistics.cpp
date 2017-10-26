#include "statistics.h"

statistics::statistics()
{
}

refer statistics::min_degree(graph G)
{
    refer i;
    refer min_deg;

    min_deg = G->n;
    for (i=0;i<G->n;i++)
    {
        if (min_deg > G->V[i].edgecount)
        {
            min_deg = G->V[i].edgecount;
        }
    }

    return min_deg;
}

refer statistics::max_degree(graph G)
{
    refer i;
    refer max_deg;

    max_deg = 0;
    for (i=0;i<G->n;i++)
    {
        if (max_deg < G->V[i].edgecount)
        {
            max_deg = G->V[i].edgecount;
        }
    }

    return max_deg;
}

double statistics::average_degree(graph G)
{
    refer i;
    double avg_deg;

    avg_deg = 0.0;
    for (i=0;i<G->n;i++)
    {
        avg_deg += (double)(G->V[i].edgecount);
    }
    avg_deg /= G->n;

    return avg_deg;
}

double statistics::degree_stdev(graph G)
{
    refer i;
    double avg_deg,stdev_deg;

    avg_deg = 0.0;
    for (i=0;i<G->n;i++)
    {
        avg_deg += (double)(G->V[i].edgecount);
    }
    avg_deg /= G->n;

    stdev_deg = 0.0;
    for (i=0;i<G->n;i++)
    {
        stdev_deg += ((double)(G->V[i].edgecount)-avg_deg)*((double)(G->V[i].edgecount)-avg_deg);
    }
    stdev_deg /= G->n;
    stdev_deg = sqrt(stdev_deg);

    return stdev_deg;
}

refer statistics::degree_distribution(graph G, refer *target)
{
    // degree distr.
    refer i, maxdeg = 0;

    for (i=0;i<G->n;i++)
    {
        target[i] = 0;
    }
    for (i=0;i<G->n;i++)
    {
        if (maxdeg < G->V[i].edgecount)
        {
            maxdeg = G->V[i].edgecount;
        }
        target[G->V[i].edgecount] += 1;
    }

    return maxdeg;
}

unsigned long long statistics::triangles(graph G)
{
    refer i,j,k;
    unsigned long long count = 0;

    for (i=0;i<G->n;i++)
    {
        for (j=0;j<G->V[i].edgecount;j++)
        {
            for (k=0;k<G->V[G->V[i].sibl[j]].edgecount;k++)
            {
                if (are_adjacent(i, G->V[G->V[i].sibl[j]].sibl[k]))
                {
                    count++;
                }
            }
        }
    }

    // each triangle is in this list in 3! = 6 permutations
    return count / 6;
}

double statistics::mean_clustering_coefficient(graph G)
{
    refer i,j,k;
    unsigned long long triangles_count = 0;
    unsigned long long connected_triplets_count = 0;
    double total_clustering_coefficient = 0.0;

    for (i=0;i<G->n;i++)
    {
        triangles_count = 0;
        connected_triplets_count = 0;
        if (G->V[i].edgecount > 1)
        {
            for (j=0;j<G->V[i].edgecount;j++)
            {
                for (k=j+1;k<G->V[i].edgecount;k++)
                {
                    if (are_adjacent(G->V[i].sibl[j], G->V[i].sibl[k]))
                    {
                        triangles_count++;
                    }
                    connected_triplets_count++;
                }
            }
            total_clustering_coefficient += (double) triangles_count / (double) connected_triplets_count;
        }
    }

    // averaging the local clustering coefficients
    return total_clustering_coefficient / (double) G->n;
}

refer statistics::girth(graph G)
{
    refer girth;
    refer v, j, v_central = 0, queue_size, queue_current, current_distance;

    refer *distances = new refer[G->n];
    refer *queue = new refer[G->n];
    refer *parents = new refer[G->n];
    bool *processed = new bool[G->n];

    // shortest cycle
    girth = G->n+1;
    for (v_central=0;v_central<G->n;v_central++)
    {
        for (v=0;v<G->n;v++)
        {
            distances[v] = 0;
            processed[v] = 0;
        }
        // breadth-first search
        queue_current = 0;
        queue_size = 1;
        queue[0] = v_central;
        while (queue_size)
        {
            v = queue[queue_current];
            current_distance = distances[v];
            queue_current++;
            queue_size--;
            processed[v] = 1;
            for (j=0;j<G->V[v].edgecount;j++)
            {
                if (v != v_central && G->V[v].sibl[j] == parents[v])
                {
                    continue;
                }
                if (! processed[G->V[v].sibl[j]])
                {
                    if (0 == distances[G->V[v].sibl[j]])
                    {
                        queue[queue_current+queue_size] = G->V[v].sibl[j];
                        queue_size++;
                    }
                    distances[G->V[v].sibl[j]] = current_distance + 1;
                    parents[G->V[v].sibl[j]] = v;
                }
                else
                {
                    if (girth > (distances[v] + distances[G->V[v].sibl[j]] + 1))
                    {
                        girth = distances[v] + distances[G->V[v].sibl[j]] + 1;
                    }
                }
            }
        }
    }

    // some longer cycle
    /*girth = G->n+1;
    for (v_central=0;v_central<G->n;v_central++)
    {
        for (v=0;v<G->n;v++)
        {
            distances[v] = 0;
            processed[v] = 0;
        }
        // breadth-first search
        queue_current = 0;
        queue_size = 1;
        queue[0] = v_central;
        while (queue_size)
        {
            v = queue[queue_current];
            current_distance = distances[v];
            queue_current++;
            queue_size--;
            processed[v] = 1;
            for (j=0;j<G->V[v].edgecount;j++)
            {
                if (v != v_central && G->V[v].sibl[j] == parents[v])
                {
                    continue;
                }
                if (! processed[G->V[v].sibl[j]])
                {
                    if (0 == distances[G->V[v].sibl[j]])
                    {
                        queue[queue_current+queue_size] = G->V[v].sibl[j];
                        queue_size++;
                        distances[G->V[v].sibl[j]] = current_distance + 1;
                    }
                    parents[G->V[v].sibl[j]] = v;
                }
                else
                {
                    if (G->n+1 == girth || girth < (distances[v] + distances[G->V[v].sibl[j]] + 1))
                    {
                        girth = distances[v] + distances[G->V[v].sibl[j]] + 1;
                    }
                }
            }
        }
    }*/

    delete[](distances);
    delete[](queue);
    delete[](parents);
    delete[](processed);

    return girth;
}

refer statistics::diameter(graph G)
{
    refer diameter;
    refer v, j, v_central = 0, queue_size, queue_current, current_distance;

    refer *distances = new refer[G->n];
    refer *queue = new refer[G->n];

    // ToDo: we should handle the case when the graph has more components; then the diameter is infinite

    diameter = 0;
    for (v_central=0;v_central<G->n;v_central++)
    {
        for (v=0;v<G->n;v++)
        {
            distances[v] = 0;
        }
        // breadth-first search
        queue_current = 0;
        queue_size = 1;
        queue[0] = v_central;
        while (queue_size)
        {
            v = queue[queue_current];
            current_distance = distances[v];
            queue_current++;
            queue_size--;
            for (j=0;j<G->V[v].edgecount;j++)
            {
                if (0 == distances[G->V[v].sibl[j]] && G->V[v].sibl[j] != v_central)
                {
                    queue[queue_current+queue_size] = G->V[v].sibl[j];
                    distances[G->V[v].sibl[j]] = current_distance + 1;
                    if (diameter < distances[G->V[v].sibl[j]])
                    {
                        diameter = distances[G->V[v].sibl[j]];
                    }
                    queue_size++;
                }
            }
        }
    }

    delete[](distances);
    delete[](queue);

    return diameter;
}

refer statistics::components(graph G)
{
    refer components;
    refer v, j, v_central = 0, queue_size, queue_current;

    bool *visited = new bool[G->n];
    refer *queue = new refer[G->n];

    for (v=0;v<G->n;v++)
    {
        visited[v] = false;
    }

    components = 0;
    for (v_central=0;v_central<G->n;v_central++)
    {
        // we start only in non-visited vertices
        if (visited[v_central])
        {
            continue;
        }
        components++;
        // breadth-first search
        queue_current = 0;
        queue_size = 1;
        queue[0] = v_central;
        while (queue_size)
        {
            v = queue[queue_current];
            queue_current++;
            queue_size--;
            for (j=0;j<G->V[v].edgecount;j++)
            {
                if (! visited[G->V[v].sibl[j]] && G->V[v].sibl[j] != v_central)
                {
                    queue[queue_current+queue_size] = G->V[v].sibl[j];
                    visited[G->V[v].sibl[j]] = true;
                    queue_size++;
                }
            }
        }
    }

    delete[](visited);
    delete[](queue);

    return components;
}
