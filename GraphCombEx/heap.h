#ifndef HEAP_H
#define HEAP_H
#include "graphs.h"

class heap
{
    public:
        heap(double values[]);
        // samotna halda
        refer Q[MAX_VERTICES];
        // D[i] obsahuje ohodnotenie i-teho elementu v halde
        double D[MAX_VERTICES];
        // H[i] obsahuje poziciu i-teho vrchola grafu v halde
        refer H[MAX_VERTICES];
        // velkost haldy
        refer heapsize;
        long n;
        void createset();
        refer left(refer i);
        refer right(refer i);
        void heapify(refer i);
        void heapify_randomized(refer i);
        void update(refer i, refer largest);
        void buildheap();
        refer extractmin();
};

#endif // HEAP_H
