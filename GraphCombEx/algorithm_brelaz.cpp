#include "algorithm_brelaz.h"

algorithm_brelaz::algorithm_brelaz()
{
}

algorithm_brelaz::~algorithm_brelaz()
{
}

refer algorithm_brelaz::brelaz_with_heap(graph G, refer *result)
{
    refer i,j,vertex,color,c;
    refer current_colors = 0;
    long updates = 0,inv_updates = 0,nv;

    priorities = new double[G->n];
    color_neighbor_matrix = new bool*[G->n+1];

    for (i=0;i<G->n;i++)
    {
        priorities[i] = - (double)(G->V[i].edgecount);
        result[i] = 0;
    }

    queue = new brelaz_heap(G, priorities);

    current_colors = 0;
    for (j=0;j<G->n;j++)
    {
        // we find out, how many candidates do we actually have
        // extraction of minimum from the heap
        vertex = queue->extractmin();
        queue->heapify(0);
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
                    color_neighbor_matrix[color] = new bool[G->n];
                    for (i=0;i<G->n;i++)
                    {
                        color_neighbor_matrix[color][i] = 0;
                    }
                }
                break;
            }
        }
        // we color the vertex
        priorities[vertex] = 1;
        result[vertex] = color;
        // all the neighbors now do have a colored neighbor
        for (i=0;i<G->V[vertex].edgecount;i++)
        {
            if (priorities[G->V[vertex].sibl[i]] <= 0)
            {
                if (color_neighbor_matrix[color][G->V[vertex].sibl[i]] == 0)
                {
                    updates++;
                    priorities[G->V[vertex].sibl[i]] -= (double)(G->n);
                    // heap part - change value and hierarchical update of the heap
                    queue->D[G->V[vertex].sibl[i]] -= (double)(G->n);
                    nv = queue->H[G->V[vertex].sibl[i]];
                    while (
                            (queue->D[queue->Q[(nv-1)/2]] > queue->D[queue->Q[nv]]) ||
                            (queue->D[queue->Q[(nv-1)/2]] == queue->D[queue->Q[nv]] && generator.random(0,1) == 1)
                          )
                    {
                        queue->update(nv,(nv-1)/2);
                        nv = (nv-1)/2;
                    }
                }
                else
                {
                    inv_updates++;
                }
                color_neighbor_matrix[color][G->V[vertex].sibl[i]]++;
            }
        }
    }

    delete(queue);
    delete[](priorities);
    for (i=1;i<=current_colors;i++)
    {
        delete[](color_neighbor_matrix[i]);
    }
    delete[](color_neighbor_matrix);

    return current_colors;
}

////////////////////
// AUXILIARY HEAP //
////////////////////

// the constructor initializes everything in the heap - including auxiliary things
brelaz_heap::brelaz_heap(graph G, double *values)
{
    refer i;
    random_generator generator;

    n = G->n;
    Q = new refer[n];
    D = new double[n];
    H = new refer[n];

    createset();
    for (i=0;i<n;i++)
    {
        D[i] = values[i] + generator.random_double();
    }
    for (i=0;i<n;i++)
    {
        H[i] = i;
    }
    buildheap();
}

brelaz_heap::~brelaz_heap()
{
    delete[](Q);
    delete[](D);
    delete[](H);
}

void brelaz_heap::createset()
{
    refer i;
    for (i=0;i<n;i++)
    {
        Q[i] = i;
    }
    heapsize = n-1;
}

refer brelaz_heap::left(refer i)
{
    return 2*i+1;
}

refer brelaz_heap::right(refer i)
{
    return 2*i+2;
}

void brelaz_heap::heapify(refer i)
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
        H[Q[i]] = largest;
        H[Q[largest]] = i;
        hlp = Q[i];
        Q[i] = Q[largest];
        Q[largest] = hlp;
        heapify(largest);
    }
}

void brelaz_heap::update(refer i, refer largest)
{
    refer hlp;
    H[Q[i]] = largest;
    H[Q[largest]] = i;
    hlp = Q[i];
    Q[i] = Q[largest];
    Q[largest] = hlp;
}

void brelaz_heap::buildheap()
{
    long i;
    for (i=(long)(heapsize/2);i>=0;i--)
    {
        heapify(i);
    }
}

refer brelaz_heap::extractmin()
{
    refer min;
    if (heapsize < 0)
    {
        return 0;
    }
    min = Q[0];
    Q[0] = Q[heapsize];
    H[Q[0]] = 0;
    heapsize--;
    return min;
}
