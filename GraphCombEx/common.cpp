#include "common.h"
#include "random_generator.h"

random_generator gen_common;

refer CommonSettings::runs_brelaz = 1;
refer CommonSettings::runs_dfs = 1;
refer CommonSettings::runs_dfs_ls = 1;
refer CommonSettings::runs_greedy_clique = 1;
refer CommonSettings::runs_greedy_dominating_set = 1;
refer CommonSettings::runs_greedy_independent_set = 1;
refer CommonSettings::runs_rls_ig_clique = 1;
refer CommonSettings::runs_rls_ig_independent_set = 1;
long long CommonSettings::crossing_minimization_time_limit = 60 * 1000;
int CommonSettings::label_font_size = 4;
int CommonSettings::vertex_radius = 3;
bool CommonSettings::highlight_coloring = true;
bool CommonSettings::highlight_longest_cycle = true;

void swap(refer *a, refer *b)
{
    refer aux;
    aux = *a;
    *a = *b;
    *b = aux;
}

int split(refer *array, int left, int right)
{
    int L,i,j;
    refer T;
    L = gen_common.random(left,right);
    swap(&array[left],&array[L]);
    T = array[left];
    i = left;
    for (j=left+1;j<=right;j++)
    {
        if (array[j] < T)
        {
            i++;
            swap(&array[i],&array[j]);
        }
    }
    swap(&array[left],&array[i]);
    return i;
}

void QuickSort(refer *array, int left, int right)
{
    if (right >= left)
    {
        int j;

        // manual implementation of the stack to avoid recursion
        int *stack_left = new int[right - left + 1];
        int *stack_right = new int[right - left + 1];
        int stack_count;
        int current_left;
        int current_right;

        stack_count = 1;
        stack_left[0] = left;
        stack_right[0] = right;

        while (0 < stack_count)
        {
            // pop
            current_left = stack_left[stack_count-1];
            current_right = stack_right[stack_count-1];
            stack_count--;
            if (current_left < current_right)
            {
                j = split(array,current_left,current_right);
                // recursion - left side
                stack_left[stack_count] = current_left;
                stack_right[stack_count] = j-1;
                // recursion - right side
                stack_left[stack_count+1] = j+1;
                stack_right[stack_count+1] = current_right;
                stack_count += 2;
            }
        }

        delete[](stack_left);
        delete[](stack_right);
    }
}

int BinarySearch(refer *array, refer key, int left, int right)
{
    int middle;
    while (right >= left)
    {
        middle = left + ((right - left) / 2);
        if (array[middle] == key)
        {
            return middle;
        }
        else if (array[middle] < key)
        {
            left = middle + 1;
        }
        else
        {
            right = middle - 1;
        }
    }
    return -1;
}

unsigned long long power(unsigned long long base, unsigned long long exponent)
{
    unsigned int i;
    unsigned long long value = 1;

    for (i=0;i<exponent;i++)
    {
        value *= base;
    }

    return value;
}
