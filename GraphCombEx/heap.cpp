#include "heap.h"
#include "graphs.h"
#include "random_generator.h"

// the constructor initializes everything in the heap - including auxiliary things
heap::heap(double values[])
{
    refer i,j,k,aux;
    random_generator generator;
    n = get_graph()->n;
    createset();
    for (i=0;i<n;i++)
    {
        D[i] = values[i] + generator.random_double();
    }
    for (i=0;i<n;i++)
    {
        H[i] = i;
    }
    /*for (i=0;i<n;i++)
    {
        j = generator.random(0,n-1);
        k = generator.random(0,n-1);
        aux = D[j];
        D[j] = D[k];
        D[k] = aux;
        aux = Q[j];
        Q[j] = Q[k];
        Q[k] = aux;
        H[Q[j]] = j;
        H[Q[k]] = k;
    }*/
    buildheap();
}

// vytvorenie prioritneho frontu
void heap::createset()
{
        refer i;
        for (i=0;i<n;i++)
        {
                Q[i] = i;
        }
        heapsize = n-1;
}

// lavy potomok
refer heap::left(refer i)
{
        return 2*i+1;
}

// pravy potomok
refer heap::right(refer i)
{
        return 2*i+2;
}

// udrziavanie haldy
void heap::heapify(refer i)
{
        refer l,r,largest;
        l = left(i);
        r = right(i);
        if (l <= heapsize && D[Q[l]] < D[Q[i]])
                largest = l;
        else
                largest = i;
        if (r <= heapsize && D[Q[r]] < D[Q[largest]])
                largest = r;
        if (largest != i)
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

// udrziavanie haldy
void heap::heapify_randomized(refer i)
{
        random_generator generator;
        refer l,r,largest;
        l = left(i);
        r = right(i);
        if (l <= heapsize && (D[Q[l]] < D[Q[i]] || (D[Q[l]] == D[Q[i]] && generator.random(0,1) == 1)))
                largest = l;
        else
                largest = i;
        if (r <= heapsize && (D[Q[r]] < D[Q[largest]] || (D[Q[r]] == D[Q[largest]] && generator.random(0,1) == 1)))
                largest = r;
        if (largest != i)
        {
                refer hlp;
                H[Q[i]] = largest;
                H[Q[largest]] = i;
                hlp = Q[i];
                Q[i] = Q[largest];
                Q[largest] = hlp;
                heapify_randomized(largest);
        }
}

// update haldy
void heap::update(refer i, refer largest)
{
        refer hlp;
        H[Q[i]] = largest;
        H[Q[largest]] = i;
        hlp = Q[i];
        Q[i] = Q[largest];
        Q[largest] = hlp;
}

// vytvorenie haldy
void heap::buildheap()
{
        refer i;
        for (i=heapsize/2;i>=0;i--)
                heapify(i);
}

// extrakcia minima
refer heap::extractmin()
{
        refer min;
        if (heapsize < 0)
                return 0;
        min = Q[0];
        Q[0] = Q[heapsize];
        H[Q[0]] = 0;
        heapsize--;
        return min;
}
