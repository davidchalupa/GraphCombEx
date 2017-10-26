#include "algorithm_greedyclique.h"

algorithm_greedyclique::algorithm_greedyclique()
{
}

algorithm_greedyclique::~algorithm_greedyclique()
{
}

refer algorithm_greedyclique::greedy_clique(graph G, refer *result)
{
    refer i,j;
    refer vertex;
    refer clique_size;
    refer neighbors_in_clique;

    is_in_clique = new bool[G->n];
    Q = new refer[G->n];
    D = new double[G->n];

    this->G = G;
    createset();

    for (i=0;i<G->n;i++)
    {
        is_in_clique[i] = false;
    }

    clique_size = 0;

    for (i=0;i<G->n;i++)
    {
        vertex = extractmin();
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
            result[clique_size] = vertex;
            clique_size++;
        }
    }

    delete[](is_in_clique);
    delete[](Q);
    delete[](D);

    return clique_size;
}

void algorithm_greedyclique::createset()
{
    refer i;
    n = G->n;
    for (i=0;i<n;i++)
    {
        Q[i] = i;
        D[i] = (double)(G->n - G->V[i].edgecount) + generator.random_double();
    }
    heapsize = n-1;
    buildheap();
}

refer algorithm_greedyclique::left(refer i)
{
    return 2*i+1;
}

refer algorithm_greedyclique::right(refer i)
{
    return 2*i+2;
}

void algorithm_greedyclique::heapify(refer i)
{
    long l,r,largest;
    l = left(i);
    r = right(i);
    if (l <= heapsize && D[Q[l]] < D[Q[i]])
    {
        largest = l;
    }
    else
    {
        largest = i;
    }
    if (r <= heapsize && D[Q[r]] < D[Q[largest]])
    {
        largest = r;
    }
    if (largest != (long) i)
    {
        refer hlp;
        hlp = Q[i];
        Q[i] = Q[largest];
        Q[largest] = hlp;
        heapify(largest);
    }
}

void algorithm_greedyclique::buildheap()
{
    long i;
    for (i=(long)(heapsize/2);i>=0;i--)
    {
        heapify(i);
    }
}

refer algorithm_greedyclique::extractmin()
{
    refer min;
    if (heapsize < 0)
    {
        return 0;
    }
    min = Q[0];
    Q[0] = Q[heapsize];
    heapsize--;
    heapify(0);
    return min;
}
