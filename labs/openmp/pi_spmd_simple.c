#include <stdio.h>
#include <omp.h>

#define NUM_THREADS 2
static long num_steps = 1000000;
double step;
int main ()
{
    int i, nthreads;
    double pi, sum[NUM_THREADS];
    double start_time, run_time;

    step = 1.0/(double) num_steps;
    omp_set_num_threads(NUM_THREADS);
    start_time = omp_get_wtime();

#pragma omp parallel
    {
        int i, id, nthrds;
        double x;
        id = omp_get_thread_num();
        nthrds = omp_get_num_threads();

        if (id == 0)
            nthreads = nthrds;

        for (i=id,sum[id]=0.0;i < num_steps; i=i + nthrds){
            x = (i-0.5)*step;
            sum[id] += 4.0/(1.0+x*x);
        }
    };

    for (i=0, pi=0.0;i<nthreads; i++) {
        pi += sum[i] * step;
    }

    run_time = omp_get_wtime() - start_time;
    printf("\n pi with %d steps is %f in %f seconds\n",num_steps,pi,run_time);
}
