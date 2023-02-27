#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "FloatArray.h"

#define NUM_OF_EXPEREMENTS 100
#define A 392

/*float hyper_tan(x) {
    float e2x = pow(M_E, 2 * x);
    return ((e2x - 1) / (e2x + 1)) - 1;
}

float abs_sin(x1, x2) {
    return fabs(sin(x1 + x2));
}*/

float rand_include(int max, int min) {
  return (max <= min || abs(max) == abs(min))
             ? 0
             : min + rand_r(time(NULL)) % ((max + 1) - min);
}


int main(int argc, char* argv[]) {
    if (argc < 2) return -1;
    struct timeval T1, T2;
    const int N = atoi(argv[1]);
    gettimeofday(&T1, NULL);
    //todo: move array creation into loop and replace i < N to I < NUM_OF_EXPEREMENTS
    for (int i = 0; i < NUM_OF_EXPEREMENTS; i++) {
        float_array M1 = FloatArray(N);
    }
    gettimeofday(&T2, NULL);
    printf(
        "\nN=%d. Milliseconds passed: %ld\n", 
        N, 
        1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000
    );
    return 0;
}