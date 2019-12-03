#include <stdio.h>
#include <omp.h>

#define NUM_THREADS 2
static long num_steps = 1000000;
double step;
int main ()
{
    int i;
    double x, pi, sum = 0.0;
    double start_time, run_time;

    step = 1.0/(double) num_steps;
    omp_set_num_threads(NUM_THREADS);
    start_time = omp_get_wtime();

#pragma omp parallel for private(x) reduction(+:sum)
        for (i = 0; i < num_steps; i++) {
            x = (i - 0.5) * step;
            sum = sum + 4.0 / (1.0 + x * x);
        }
        pi += sum * step;
        run_time = omp_get_wtime() - start_time;
        printf("\n pi with %d steps is %f in %f seconds\n",num_steps,pi,run_time);
}