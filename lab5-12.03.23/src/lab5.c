#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdbool.h>
#include <pthread.h>
#ifdef _OPENMP
#include "omp.h"
#endif
#include "FloatArray.h"

#define A 392

#if defined(_OPENMP)
#define TIME double
#define TICK(t) t = omp_get_wtime()
#define TOCK(s) ({ TIME f; TICK(f); (f - s) * 1000; })
#else
#define TIME struct timeval
#define TICK(t) gettimeofday(&t, NULL)
#define TOCK(s) ({ TIME f; TICK(f); 1000 * (f.tv_sec - s.tv_sec) + (f.tv_usec - s.tv_usec) / 1000; })
#endif



unsigned short progress = 0;
int num_of_experements = 0;
unsigned short num_of_cores = -1;
const unsigned short DELAY_SEC = 1;
pthread_t progress_t;


void* print_progress(void* arg) {
    while (progress < num_of_experements) {
        sleep(DELAY_SEC);
        float percentage = (float) progress / (float) num_of_experements * 100.0;
        printf("%.0f%\n", percentage);
    }
    return NULL;
}

float hyper_tan_minus1(float x) { return tanh(x) + 1; }  // special function discussed personally with the teacher 
float to_negative0_1(float x) { return x / 1000 * -1; }  // special function discussed personally with the teacher 
float abs_sin(float val1, float val2) { return fabs(sin(val1 + val2)); }
float pair_min(float val1, float val2) { return val1 < val2 ? val1 : val2; }
bool not_null(float val) { return val != 0 && val != NAN; }
bool sum_not_null_predicate(float it, float predicate_arg) { return (int)(it / predicate_arg) == 0; }


// argv[0]: programm name; argv[1]: size of test array; argv[2]: number of experements
int main(int argc, char* argv[]) {
    if (argc < 3) return -1;
    const int N = atoi(argv[1]);
    num_of_experements = atoi(argv[2]);
    TIME* times_start = malloc(6 * sizeof(TIME));
    #if defined(_OPENMP)
    omp_set_num_threads(16);  // equals to number of processor's threads
    times_start[5] = omp_get_wtime();
    #else
    gettimeofday(&times_start[5], NULL);
    #endif
    long* times_finish = malloc(6 * sizeof(long));
    pthread_create(&progress_t, NULL, print_progress, NULL);
    for (int i = 0; i < num_of_experements; i++) {

        // GENERATE[0]
        TICK(times_start[0]);
        float_array M1 = FloatArray(N);
        float_array M2 = FloatArray(N / 2);
        fill_rand(&M1, A, 1);
        map(&M1, to_negative0_1); // special transform discussed personally with the teacher 
        fill_rand(&M2, 10 * A, A);
        times_finish[0] += TOCK(times_start[0]);

        // MAP[1]
        TICK(times_start[1]);
        map(&M1, hyper_tan_minus1);
        float_array M2_COPY = clone(&M2);
        for (unsigned long i = 0; i < M2.size; i++) {
            float m2i_cur = M2_COPY.begin_ptr[i];
            float m2i_prev = i == 0 ? 0 : M2_COPY.begin_ptr[i - 1];
            M2.begin_ptr[i] = abs_sin(m2i_cur, m2i_prev);
        }
        times_finish[1] += TOCK(times_start[1]);
        clean(&M2_COPY);

        // MERGE[2]
        TICK(times_start[2]);
        merge(&M1, &M2, pair_min);
        times_finish[2] += TOCK(times_start[2]);

        // SORT[3]
        TICK(times_start[3]);
        sort(&M2);
        times_finish[3] += TOCK(times_start[3]);

        // REDUCE[4]
        TICK(times_start[4]);
        sort(&M2);
        float_array not_null_arr = filter(&M2, not_null);
        float min_not_null = min(&not_null_arr);
        float sum = sum_by(&M2, sum_not_null_predicate, min_not_null);
        times_finish[4] += TOCK(times_start[4]);
        
        clean(&M1);
        clean(&M2);
        progress++;
    }

    // TOTAL[5]
    pthread_join(progress_t, NULL);
    times_finish[5] += TOCK(times_start[5]);

    for (unsigned long i = 0; i < 6; i++) {
        printf("%d", times_finish[i]);
        if (i != 6 - 1) 
            printf(", ");
    }
    return 0;
}