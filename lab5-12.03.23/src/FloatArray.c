#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "FloatArray.h"

// join ParallelITERator
#define JOIN_PITER() for (long i = 0; i < piter.num_of_thrs; i++) pthread_join(piter.thrs[i], NULL)

// create ParallelITERator
#define CREATE_PITER(f) {\
    struct pair range = get_range(&piter, i);\
    args->from = range.first;\
    args->to = range.second;\
    pthread_create(&piter.thrs[i], NULL, f, (void*) args);\
}

__thread unsigned int seed = 0;

unsigned int hash(unsigned int arg) { return ((int) &arg >> 4) * arg; }

float_array FloatArray(unsigned long size) {
    float_array new;
    new.size = size;
    new.begin_ptr = malloc(size * sizeof(float));
    new.engaged = 0;
    return new;
}


struct par_iter get_parallel_iterator(long arr_size) {
    long num_of_cores = sysconf(_SC_NPROCESSORS_ONLN);
    long default_chunk = arr_size / num_of_cores;
    long last_chunk = default_chunk + (arr_size % num_of_cores);
    pthread_t* thrs;
    int n_thrs = sysconf(_SC_NPROCESSORS_ONLN);
    thrs = malloc(n_thrs * sizeof(pthread_t));
    return (struct par_iter){ default_chunk, last_chunk, n_thrs, thrs };
}

struct pair get_range(struct par_iter* piter, long i) {
    long from = i * piter->default_chunk_size;
    long to = i * piter->default_chunk_size + (piter->num_of_thrs - 1 != i ? piter->default_chunk_size : piter->last_chunk_size);
    return (struct pair){ from, to };
}


float_array* push(float_array* restrict arr, float val) {
    arr->begin_ptr[arr->engaged] = val;
    arr->engaged = arr->engaged + 1;
    return arr;
}


    struct __map_pthread_args { float_array* restrict arr; unsigned long from; unsigned long to; float(* transform)(float); };

    void* __map_pthread(void* args) {
        struct __map_pthread_args* args_st = (struct __map_pthread_args*) args;
        for (unsigned long i = args_st->from; i < args_st->to; i++)
            args_st->arr->begin_ptr[i] = args_st->transform(args_st->arr->begin_ptr[i]);
        return NULL;
    }

float_array* map(float_array* restrict arr, float(* transform)(float)) {
    #ifdef _OPENMP
    #pragma omp parallel for
    for (unsigned long i = 0; i < arr->size; i++)
        arr->begin_ptr[i] = transform(arr->begin_ptr[i]);
    #else
    struct par_iter piter = get_parallel_iterator(arr->size);
    for (long i = 0; i < piter.num_of_thrs; i++) {
        struct __map_pthread_args* args = malloc(sizeof(struct __map_pthread_args));
        args->arr = arr;
        args->transform = transform;
        CREATE_PITER(__map_pthread);
    }
    JOIN_PITER();
    #endif
    return arr;
}


    struct __merge_pthread_args { float_array* restrict from_arr; float_array* restrict to_arr; unsigned long from; unsigned long to; float(* transform)(float, float); };

    void* __merge_pthread(void* args) {
        struct __merge_pthread_args* args_st = (struct __merge_pthread_args*) args;
        for (unsigned long i = args_st->from; i < args_st->to; i++)
            args_st->to_arr->begin_ptr[i] = args_st->transform(args_st->from_arr->begin_ptr[i], args_st->to_arr->begin_ptr[i]);
        return NULL;
    }

float_array* merge(float_array* restrict from, float_array* restrict to, float(* transform)(float, float)) {
    #ifdef _OPENMP
    #pragma omp parallel for
    for (unsigned long i = 0; i < to->size; i++) 
        to->begin_ptr[i] = transform(from->begin_ptr[i], to->begin_ptr[i]);
    #else
    struct par_iter piter = get_parallel_iterator(from->size);
    for (long i = 0; i < piter.num_of_thrs; i++) {
        struct __merge_pthread_args* args = malloc(sizeof(struct __merge_pthread_args));
        args->from_arr = from;
        args->to_arr = to;
        args->transform = transform;
        CREATE_PITER(__merge_pthread);
    }
    JOIN_PITER();
    #endif
    return to;
}


    struct __fill_rand_pthread_args { float_array* restrict arr; int max; int min; unsigned long from; unsigned long to; };

    void* __fill_rand_pthread(void* args) {
        struct __fill_rand_pthread_args* args_st = (struct __fill_rand_pthread_args*) args;
        for (unsigned long i = args_st->from; i < args_st->to; i++) {
            seed = i;
            srand(hash(seed));
            args_st->arr->begin_ptr[i] = (args_st->max <= args_st->min || abs(args_st->max) == abs(args_st->min)) ? 0 : args_st->min + rand_r(&seed) % ((args_st->max + 1) - args_st->min);
        }
        return NULL;
    }

float_array* fill_rand(float_array* restrict arr, int max, int min) {
    #ifdef _OPENMP
    #pragma omp parallel for
    for (unsigned long i = 0; i < arr->size; i++) {
        seed = i;
        srand(hash(seed));
        arr->begin_ptr[i] = (max <= min || abs(max) == abs(min)) ? 0 : min + rand_r(&seed) % ((max + 1) - min);
    }
    #else
    struct par_iter piter = get_parallel_iterator(arr->size);
    for (long i = 0; i < piter.num_of_thrs; i++) {
        struct __fill_rand_pthread_args* args = malloc(sizeof(struct __fill_rand_pthread_args));
        args->arr = arr;
        args->max = max;
        args->min = min;
        CREATE_PITER(__fill_rand_pthread);
    }
    JOIN_PITER();
    #endif
    return arr;
}


    struct __clone_pthread_args { float_array* restrict orig; float_array* restrict copy; unsigned long from; unsigned long to; };

    void* __clone_pthread(void* args) {
        struct __clone_pthread_args* args_st = (struct __clone_pthread_args*) args;
        for (unsigned long i = args_st->from; i < args_st->to; i++) 
            push(&args_st->copy, args_st->orig->begin_ptr[i]);
        return NULL;
    }

float_array clone(float_array* restrict arr) {
    float_array new = FloatArray(arr->size);
    #ifdef _OPENMP
    #pragma omp parallel for
    for (unsigned long i = 0; i < arr->size; i++) 
        push(&new, arr->begin_ptr[i]);
    #else
    struct par_iter piter = get_parallel_iterator(arr->size);
    for (long i = 0; i < piter.num_of_thrs; i++) {
        struct __clone_pthread_args* args = malloc(sizeof(struct __clone_pthread_args));
        args->orig = arr;
        args->copy = &new;
        CREATE_PITER(__clone_pthread);
    }
    JOIN_PITER();
    #endif
    return new;
}

    struct __filter_pthread_args { float_array* restrict orig; float_array* restrict new; bool(* check)(float); unsigned long from; unsigned long to; };

    void* __filter_pthread(void* args) {
        struct __filter_pthread_args* args_st = (struct __filter_pthread_args*) args;
        for (unsigned long i = args_st->from; i < args_st->to; i++)
            if (args_st->check(args_st->orig->begin_ptr[i]) == true) 
                push(args_st->new, args_st->orig->begin_ptr[i]);
        return NULL;
    }

float_array filter(float_array* restrict arr, bool(* check)(float)) {
    unsigned long new_arr_size = 0;
    for (unsigned long i = 0; i < arr->size; i++) 
        if (check(arr->begin_ptr[i]) == true) 
            new_arr_size++;
    float_array new = FloatArray(new_arr_size);
    #ifdef _OPENMP
    #pragma omp parallel for
    for (unsigned long i = 0; i < arr->size; i++) 
        if (check(arr->begin_ptr[i]) == true) 
            push(&new, arr->begin_ptr[i]);
    #else
    struct par_iter piter = get_parallel_iterator(arr->size);
    for (long i = 0; i < piter.num_of_thrs; i++) {
        struct __filter_pthread_args* args = malloc(sizeof(struct __filter_pthread_args));
        args->orig = arr;
        args->new = &new;
        args->check = check;
        CREATE_PITER(__filter_pthread);
    }
    JOIN_PITER();
    #endif
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
        if (i != arr->size - 1) 
            printf(", ");
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
