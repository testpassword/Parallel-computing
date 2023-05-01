#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "omp.h"

__thread unsigned int seed = 0;

void map(float* restrict arr, int size, float(* transform)(float)) {
    #pragma omp parallel for
    for (int i = 0; i < size; i++)
        arr[i] = transform(arr[i]);
}

float min(float* restrict arr, int size) {
    float min = arr[0];
    #pragma omp parallel for reduction(min:min)
    for (int i = 0; i < size; i++)
        min = arr[i] < min ? arr[i] : min;
    return min;
}

 void fill_rand(float* restrict arr, int size, int max, int min) {
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        seed = i;
        arr[i] = (max <= min || abs(max) == abs(min)) ? 0 : min + rand_r(&seed) % ((max + 1) - min);
    }
}

float hyper_tan_minus1(float x) { return tanh(x) + 1; }

int main(int argc, char* argv[]) {
    const int MAX_V = 1000;
    const int MIN_V = 1;
    const int SIZE = 999999;
    omp_set_num_threads(16);
    double start, finish;
    long exec_time;
    float* test_arr1 = malloc(SIZE * sizeof(float));
    start = omp_get_wtime();
    fill_rand(test_arr1, SIZE, MAX_V, MIN_V);
    map(test_arr1, 100, hyper_tan_minus1);
    min(test_arr1, 100);
    finish = omp_get_wtime();
    exec_time = (finish - start) * SIZE;
    printf("ms with #pragma omp parallel for taken == %ld\n", exec_time);
    float* test_arr2 = malloc(SIZE * sizeof(float));
    float local_min = test_arr2[0];
    start = omp_get_wtime();
    #pragma omp parallel sections
    {
        #pragma omp section
        for (int i = 0; i < SIZE; i++) {
            seed = i;
            test_arr2[i] = (MAX_V <= MIN_V || abs(MAX_V) == abs(MIN_V)) ? 0 : MIN_V + rand_r(&seed) % ((MAX_V + 1) - MIN_V);
        }
        #pragma omp section
        for (int i = 0; i < SIZE; i++)
            test_arr2[i] = hyper_tan_minus1(test_arr2[i]);
        #pragma omp section
        for (int i = 0; i < SIZE; i++)
            local_min = test_arr2[i] < local_min ? test_arr2[i] : local_min;
    }
    finish = omp_get_wtime();
    exec_time = (finish - start) * SIZE;
    printf("ms with #pragma omp section taken == %ld\n", exec_time);
    return 0;
}