#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdbool.h>
#include <omp.h>
#include "FloatArray.h"

#define NUM_OF_EXPEREMENTS 100
#define A 392


float hyper_tan_minus1(float x) { return tanh(x) + 1; }  // special function discussed personally with the teacher 
float to_negative0_1(float x) { return x / 1000 * -1; }  // special function discussed personally with the teacher 
float abs_sin(float val1, float val2) { return fabs(sin(val1 + val2)); }
float pair_min(float val1, float val2) { return val1 < val2 ? val1 : val2; }
bool not_null(float val) { return val != 0 && val != NAN; }

int main(int argc, char* argv[]) {
    if (argc < 2) return -1;
    struct timeval T1, T2;
    const int N = atoi(argv[1]);
    #if defined(_OPENMP)
    omp_set_num_threads(16);  // equals to number of processor's threads
    #endif
    gettimeofday(&T1, NULL);
    for (int i = 0; i < NUM_OF_EXPEREMENTS; i++) {
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
        #if defined(CHUNK_SIZE)
        #pragma omp parallel for default(none) shared(M2, min_not_null) schedule(SCHEDULE_TYPE, CHUNK_SIZE) reduction(+:sum)
        #else
        #pragma omp parallel for default(none) shared(M2, min_not_null) schedule(auto) reduction(+:sum)
        #endif
        for (unsigned long i = 0; i < M2.size; i++) {
            float it = M2.begin_ptr[i];
            if ((int)(it / min_not_null) == 0) sum += sin(it);
        }
        clean(&M1);
        clean(&M2);
        //printf("Run[%d] = %f\n", i, sum);
    }
    gettimeofday(&T2, NULL);
    printf(
        "N=%d. Milliseconds passed: %ld\n", 
        N, 
        1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000
    );
    return 0;
}
