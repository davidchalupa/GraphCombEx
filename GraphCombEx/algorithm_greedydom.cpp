#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <numeric>

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

refer algorithm_greedydom::dom_lower_bound(graph G)
{
    // This computes a lower bound for the domination number by greedily constructing a
    // feasible solution to the dual fractional dominating set problem.
    // Non-negative weights are assigned to vertices such that the sum of weights in any closed
    // neighborhood never exceeds 1.0, initially prioritizing low-degree vertices.
    if (G->n == 0)
    {
        return 0;
    }

    std::vector<double> y(G->n, 0.0);
    std::vector<double> current_load(G->n, 0.0);

    const double epsilon = 1e-12;
    const double limit = 1.0 - epsilon;

    // create an ordering of vertices based on degree (ascending)
    // heuristic: small neighborhoods are easier to "fill" without
    // blocking too many neighboring constraints
    std::vector<refer> order(G->n);
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](refer a, refer b) {
        return G->V[a].edgecount < G->V[b].edgecount;
    });

    // multi-pass greedy packing
    // we use the degree-ascending order to maximize the packing
    int num_passes = 5;
    for (int pass = 0; pass < num_passes; pass++)
    {
        bool changed = false;

        for (refer i : order) {
            // calculate max allowable increase for y[i]
            // for all j in N[i]: y[i] + load[j] <= 1.0
            double max_increase = limit - current_load[i];
            if (max_increase <= epsilon)
            {
                continue;
            }

            for (refer k = 0; k < G->V[i].edgecount; k++)
            {
                int neighbor = G->V[i].sibl[k];
                double slack = limit - current_load[neighbor];
                if (slack < max_increase) max_increase = slack;
                if (max_increase <= epsilon)
                {
                    break;
                }
            }

            if (max_increase > epsilon)
            {
                y[i] += max_increase;
                current_load[i] += max_increase;
                for (refer k = 0; k < G->V[i].edgecount; k++)
                {
                    int neighbor = G->V[i].sibl[k];
                    current_load[neighbor] += max_increase;
                }
                changed = true;
            }
        }

        // on subsequent passes, we can shuffle the order slightly
        if (! changed)
        {
            break;
        }
        std::random_shuffle(order.begin(), order.end());
    }

    // final summation
    double lower_bound = 0.0;
    for (double val : y)
    {
        lower_bound += val;
    }

    return (refer) ceil(lower_bound);
}
