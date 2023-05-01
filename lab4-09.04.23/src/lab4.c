#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdbool.h>
#ifdef _OPENMP
#include "omp.h"
#endif
#include <pthread.h>
#include "FloatArray.h"

#define A 392
#define DELAY 1
#define MARK3_NUM_OF_EXPEREMENTS 100
#define MARK4_NUM_OF_EXPEREMENTS 10

int progress = 0;
int num_of_experements = 0;

void* print_progress(void* arg) {
    while (progress < num_of_experements) {
        sleep(DELAY);
        float percentage = (float) progress / (float) num_of_experements * 100.0;
        printf("%.0f%\n", percentage);
    }
    return NULL;
}

long calc_exec_time(
    #if defined(_OPENMP)
    double start
    #else
    struct timeval start
    #endif
) {
    long exec_time;
    #if defined(_OPENMP)
    double final = omp_get_wtime();
    exec_time = (final - start) * 1000;
    #else
    struct timeval final;
    gettimeofday(&final, NULL);
    exec_time = 1000 * (final.tv_sec - start.tv_sec) + (final.tv_usec - start.tv_usec) / 1000;
    #endif
    return exec_time;
}

float hyper_tan_minus1(float x) { return tanh(x) + 1; }  // special function discussed personally with the teacher 
float to_negative0_1(float x) { return x / 1000 * -1; }  // special function discussed personally with the teacher 
float abs_sin(float val1, float val2) { return fabs(sin(val1 + val2)); }
float pair_min(float val1, float val2) { return val1 < val2 ? val1 : val2; }
bool not_null(float val) { return val != 0 && val != NAN; }


// argv[0]: programm name; argv[1]: size of test array; argv[2]: number of experements
int main(int argc, char* argv[]) {
    if (argc < 3) return -1;
    const int N = atoi(argv[1]);
    num_of_experements = atoi(argv[2]);
    long* iterations_exec_time = malloc(MARK4_NUM_OF_EXPEREMENTS * sizeof(long));
    #if defined(_OPENMP)
    omp_set_num_threads(16);  // equals to number of processor's threads
    double start;
    start = omp_get_wtime();
    #else
    struct timeval start;
    gettimeofday(&start, NULL);
    #endif
    pthread_t progress_t;
    pthread_create(&progress_t, NULL, print_progress, NULL);
    for (int i = 0; i < num_of_experements; i++) {
        if (num_of_experements == MARK4_NUM_OF_EXPEREMENTS) {
            #if defined(_OPENMP) 
            start = omp_get_wtime();
            #else
            gettimeofday(&start, NULL);
            #endif
        }
        // GENERATE
        float_array M1 = FloatArray(N);
        float_array M2 = FloatArray(N / 2);
        fill_rand(&M1, A, 1);
        map(&M1, to_negative0_1); // special transform discussed personally with the teacher 
        fill_rand(&M2, 10 * A, A);
        // MAP
        map(&M1, hyper_tan_minus1);
        float_array M2_COPY = clone(&M2);
        for (unsigned long i = 0; i < M2.size; i++) {
            float m2i_cur = M2_COPY.begin_ptr[i];
            float m2i_prev = i == 0 ? 0 : M2_COPY.begin_ptr[i - 1];
            M2.begin_ptr[i] = abs_sin(m2i_cur, m2i_prev);
        }
        clean(&M2_COPY);
        // MERGE && SORT
        sort(merge(&M1, &M2, pair_min));
        // REDUCE
        float_array not_null_arr = filter(&M2, not_null);
        float min_not_null = min(&not_null_arr);
        float sum = 0;
        // not necessary to wrap all pragmas, it will be raised as warning if openmd not supported
        #pragma omp parallel for reduction(+:sum)
        for (unsigned long i = 0; i < M2.size; i++) {
            float it = M2.begin_ptr[i];
            if ((int)(it / min_not_null) == 0) sum += sin(it);
        }
        clean(&M1);
        clean(&M2);
        progress++;
        if (num_of_experements == MARK4_NUM_OF_EXPEREMENTS) iterations_exec_time[i] = calc_exec_time(start);
    }
    pthread_join(progress_t, NULL);
    if (num_of_experements == MARK4_NUM_OF_EXPEREMENTS) {
        printf("[");
        for (long j = 0; j < MARK4_NUM_OF_EXPEREMENTS; j++) {
            printf("%d", iterations_exec_time[j]);
            if (j != MARK4_NUM_OF_EXPEREMENTS - 1 ) printf("; ");
        }
        printf("]\n");
    } else printf("N=%d. Milliseconds passed: %ld\n", N, calc_exec_time(start));
    return 0;
}