// SCHEDULE_TYPE and CHUNK_SIZE will be written to this file in build script

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef _OPENMP
#include <omp.h>
#endif

__thread unsigned int seed = 0;

unsigned int hash(unsigned int arg) { return ((int) &arg >> 4) * arg; }


typedef struct float_array {
    float* begin_ptr;
    unsigned long size;
    unsigned long engaged;
} float_array;

float_array FloatArray(unsigned long size) {
    float_array new;
    new.size = size;
    new.begin_ptr = malloc(size * sizeof(float));
    new.engaged = 0;
    return new;
}

float_array* push(float_array* restrict arr, float val) {
    arr->begin_ptr[arr->engaged] = val;
    arr->engaged = arr->engaged + 1;
    return arr;
}

float_array* map(float_array* restrict arr, float(* transform)(float)) {
    #pragma omp parallel for
    for (unsigned long i = 0; i < arr->size; i++)
        arr->begin_ptr[i] = transform(arr->begin_ptr[i]);
    return arr;
}

float_array* merge(float_array* restrict from, float_array* restrict to, float(* transform)(float, float)) {
    #pragma omp parallel for
    for (unsigned long i = 0; i < to->size; i++) 
        to->begin_ptr[i] = transform(from->begin_ptr[i], to->begin_ptr[i]);
    return to;
}

float_array* fill_rand(float_array* restrict arr, int max, int min) {
    #pragma omp parallel for
    for (unsigned long i = 0; i < arr->size; i++) {
        seed = i;
        srand(hash(seed));
        arr->begin_ptr[i] = (max <= min || abs(max) == abs(min)) ? 0 : min + rand_r(&seed) % ((max + 1) - min);
    }
    return arr;
}

float_array clone(float_array* restrict arr) {
    float_array new = FloatArray(arr->size);
    #pragma omp parallel for
    for (unsigned long i = 0; i < arr->size; i++) 
        push(&new, arr->begin_ptr[i]);
    return new;
}

float_array filter(float_array* restrict arr, bool(* check)(float)) {
    unsigned long new_arr_size = 0;
    for (unsigned long i = 0; i < arr->size; i++) 
        if (check(arr->begin_ptr[i]) == true) 
            new_arr_size++;
    float_array new = FloatArray(new_arr_size);
    #pragma omp parallel for
    for (unsigned long i = 0; i < arr->size; i++) 
        if (check(arr->begin_ptr[i]) == true) 
            push(&new, arr->begin_ptr[i]);
    return new;
}

float min(float_array* restrict arr) {
    float min = arr->begin_ptr[0];
    #pragma omp parallel for reduction(min:min)
    for (unsigned long i = 0; i < arr->size; i++)
        min = arr->begin_ptr[i] < min ? arr->begin_ptr[i] : min;
    return min;
}

void clean(float_array* restrict arr) {
    free(arr->begin_ptr);
}

float_array* print(float_array* restrict arr) {
    printf("[");
    for (unsigned long i = 0; i < arr->size; i++) {
        printf("%.6f", arr->begin_ptr[i]);
        if (i != arr->size - 1) printf(", ");
    }
    printf("]\n");
    return arr;
}

// sort algorithm was replaced because my original algorithm uses recursion, but it's not recommended to use OpenMP with it  
float_array* sort(float_array* restrict arr) {
    void __swap(float* a, float* b);
    int half = (int) arr->size / 2;
    #pragma omp parallel sections
    {
        #pragma omp section
        for (int i = 0; i < half; i++)
            for (int j = 0; j < half; j++)
                if (arr->begin_ptr[j] > arr->begin_ptr[j + 1])
                    __swap(&arr->begin_ptr[j], &arr->begin_ptr[j + 1]);
        #pragma omp section
        for (int i = half; i < arr->size - 1; i++)
            for (int j = half; j < arr->size - 1; j++)
                if (arr->begin_ptr[j] > arr->begin_ptr[j + 1])
                    __swap(&arr->begin_ptr[j], &arr->begin_ptr[j + 1]);
    }    
    return arr;
}
    void __swap(float* a, float* b) {
        float temp = *a;
        *a = *b;
        *b = temp;
    }
