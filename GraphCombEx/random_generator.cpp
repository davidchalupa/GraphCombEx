#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "random_generator.h"
#define RAND_MAX_LONG ((long long)(RAND_MAX)+1)*((long long)(RAND_MAX)+1)-1

random_generator::random_generator()
{
    temp_gauss = 0;
}

// uniformly random long long from <l,h>
long long random_generator::random(long l, long h)
{
    range = (h-l)+1;
    if (range < RAND_MAX)
    {
        rnd = l+(long long)(range*rand()/(RAND_MAX + 1.0));
    }
    else
    {
        low = rand()*(RAND_MAX + 1.0);
        low += rand();
        rnd = l+(long long)(((long long)(range)*(long long)(low))/(RAND_MAX_LONG + 1.0));
    }
    return rnd;
}

// uniformly random double from <0,1>
double random_generator::random_double()
{
    double rnd_double;
    double rnd;
    rnd = random(0,RAND_MAX);
    rnd_double = rnd / RAND_MAX;
    return rnd_double;
}

// Gaussian random with mean 0 and stdev 1 using Box-Muller algorithm
double random_generator::random_gauss()
{
    double v1;
    double v2;
    double r;
    double fac;
    double gauss;
    if (temp_gauss == 0)
    {
        do
        {
            v1 = 2.0*random_double() - 1.0;
            v2 = 2.0*random_double() - 1.0;
            r = v1*v1 + v2*v2;
        }
        while (r >= 1.0);
        fac = sqrt(-2.0*log(r)/r);
        save_gauss = v1*fac;
        gauss = v2*fac;
        temp_gauss = 1;
    }
    else
    {
        temp_gauss = 0;
        gauss = save_gauss;
    }
    return gauss;
}
