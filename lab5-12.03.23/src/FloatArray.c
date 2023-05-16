#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "FloatArray.h"
#include "PIter.h"

__thread unsigned int seed = 0;

unsigned int hash(unsigned int arg) { return ((int) &arg >> 4) * arg; }

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
    
    struct __map_pthread_args { float_array* restrict arr; unsigned long from; unsigned long to; float(* transform)(float); };

    void* __map_pthread_handler(void* args) {
        struct __map_pthread_args* args_st = (struct __map_pthread_args*) args;
        FOR_RANGE
            args_st->arr->begin_ptr[i] = args_st->transform(args_st->arr->begin_ptr[i]);
        return NULL;
    };

    #ifdef _OPENMP
    #pragma omp parallel for schedule(static)
    for (unsigned long i = 0; i < arr->size; i++)
        arr->begin_ptr[i] = transform(arr->begin_ptr[i]);
    #else
    PREPARE_PITER {
        struct __map_pthread_args* args = malloc(sizeof(struct __map_pthread_args));
        args->arr = arr;
        args->transform = transform;
        CREATE_PITER(__map_pthread_handler);
    }
    JOIN_PITER();
    #endif
    return arr;
}


float_array* merge(float_array* restrict arr, float_array* restrict to, float(* transform)(float, float)) {

    struct __merge_pthread_args { float_array* restrict from_arr; float_array* restrict to_arr; unsigned long from; unsigned long to; float(* transform)(float, float); };

    void* __merge_pthread(void* args) {
        struct __merge_pthread_args* args_st = (struct __merge_pthread_args*) args;
        FOR_RANGE
            args_st->to_arr->begin_ptr[i] = args_st->transform(args_st->from_arr->begin_ptr[i], args_st->to_arr->begin_ptr[i]);
        return NULL;
    };

    #ifdef _OPENMP
    #pragma omp parallel for schedule(static)
    for (unsigned long i = 0; i < to->size; i++) 
        to->begin_ptr[i] = transform(arr->begin_ptr[i], to->begin_ptr[i]);
    #else
    PREPARE_PITER {
        struct __merge_pthread_args* args = malloc(sizeof(struct __merge_pthread_args));
        args->from_arr = arr;
        args->to_arr = to;
        args->transform = transform;
        CREATE_PITER(__merge_pthread);
    }
    JOIN_PITER();
    #endif
    return to;
}
    

float_array* fill_rand(float_array* restrict arr, int max, int min) {

    struct __fill_rand_pthread_args { float_array* restrict arr; int max; int min; unsigned long from; unsigned long to; };

    void* __fill_rand_pthread(void* args) {
        struct __fill_rand_pthread_args* args_st = (struct __fill_rand_pthread_args*) args;
        FOR_RANGE {
            seed = i;
            srand(hash(seed));
            args_st->arr->begin_ptr[i] = (args_st->max <= args_st->min || abs(args_st->max) == abs(args_st->min)) ? 0 : args_st->min + rand_r(&seed) % ((args_st->max + 1) - args_st->min);
        }
        return NULL;
    };

    #ifdef _OPENMP
    #pragma omp parallel for schedule(static)
    for (unsigned long i = 0; i < arr->size; i++) {
        seed = i;
        srand(hash(seed));
        arr->begin_ptr[i] = (max <= min || abs(max) == abs(min)) ? 0 : min + rand_r(&seed) % ((max + 1) - min);
    }
    #else
    PREPARE_PITER {
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


float_array clone(float_array* restrict arr) {

    struct __clone_pthread_args { float_array* restrict orig; float_array* restrict copy; unsigned long from; unsigned long to; };

    void* __clone_pthread(void* args) {
        struct __clone_pthread_args* args_st = (struct __clone_pthread_args*) args;
        FOR_RANGE
            push(args_st->copy, args_st->orig->begin_ptr[i]);
        return NULL;
    };

    float_array new = FloatArray(arr->size);
    #ifdef _OPENMP
    #pragma omp parallel for schedule(static)
    for (unsigned long i = 0; i < arr->size; i++) 
        push(&new, arr->begin_ptr[i]);
    #else
    PREPARE_PITER {
        struct __clone_pthread_args* args = malloc(sizeof(struct __clone_pthread_args));
        args->orig = arr;
        args->copy = &new;
        CREATE_PITER(__clone_pthread);
    }
    JOIN_PITER();
    #endif
    return new;
}
    

float_array filter(float_array* restrict arr, bool(* check)(float)) {

    struct __filter_pthread_args { float_array* restrict orig; float_array* restrict new; bool(* check)(float); unsigned long from; unsigned long to; };

    void* __filter_pthread(void* args) {
        struct __filter_pthread_args* args_st = (struct __filter_pthread_args*) args;
        FOR_RANGE
            if (args_st->check(args_st->orig->begin_ptr[i]) == true) 
                push(args_st->new, args_st->orig->begin_ptr[i]);
        return NULL;
    };
    
    unsigned long new_arr_size = 0;
    for (unsigned long i = 0; i < arr->size; i++) 
        if (check(arr->begin_ptr[i]) == true) 
            new_arr_size++;
    float_array new = FloatArray(new_arr_size);
    #ifdef _OPENMP
    #pragma omp parallel for schedule(static)
    for (unsigned long i = 0; i < arr->size; i++) 
        if (check(arr->begin_ptr[i]) == true) 
            push(&new, arr->begin_ptr[i]);
    #else
    PREPARE_PITER {
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

    float* __min_thrs_work_result;

    struct __min_pthread_args { float_array* restrict arr; unsigned long from; unsigned long to; unsigned long chunk_number; };

    void* __min_pthread(void* args) {
        struct __min_pthread_args* args_st = (struct __min_pthread_args*) args;
        float local_min = args_st->arr->begin_ptr[args_st->from];
        FOR_RANGE
            local_min = args_st->arr->begin_ptr[i] < local_min ? args_st->arr->begin_ptr[i] : local_min;
        __min_thrs_work_result[args_st->chunk_number] = local_min;
        return NULL;
    };

    #ifdef _OPENMP
    float min = arr->begin_ptr[0];
    #pragma omp parallel for reduction(min:min) schedule(static)
    for (unsigned long i = 0; i < arr->size; i++)
        min = arr->begin_ptr[i] < min ? arr->begin_ptr[i] : min;
    #else
    long num_of_threads = sysconf(_SC_NPROCESSORS_ONLN);
    __min_thrs_work_result = malloc(num_of_threads * sizeof(float));
    PREPARE_PITER {
        struct __min_pthread_args* args = malloc(sizeof(struct __min_pthread_args));
        args->arr = arr;
        args->chunk_number = i;
        CREATE_PITER(__min_pthread);
    }
    JOIN_PITER();
    float min = __min_thrs_work_result[0];
    for (unsigned long j = 0; j < num_of_threads; j++)
        min = __min_thrs_work_result[j] < min ? __min_thrs_work_result[j] : min;
    #endif
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


float_array* sort(float_array* restrict arr) {
    void __swap(float* a, float* b);
    void __heapify(float arr[], int N, int i);
    for (int i = arr->size / 2 - 1; i >= 0; i--) 
        __heapify(arr->begin_ptr, arr->size, i);
    for (int i = arr->size - 1; i >= 0; i--) {
        __swap(&arr->begin_ptr[0], &arr->begin_ptr[i]);
        __heapify(arr->begin_ptr, i, 0);
    }
    return arr;
}
    void __swap(float* a, float* b) {
        float temp = *a;
        *a = *b;
        *b = temp;
    }
    void __heapify(float arr[], int N, int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < N && arr[left] > arr[largest]) largest = left;
        if (right < N && arr[right] > arr[largest]) largest = right;
        if (largest != i) {
            __swap(&arr[i], &arr[largest]);
            __heapify(arr, N, largest);
        }
    }
    

float sum_by(float_array* restrict arr, bool(* predicate)(float, float), float predicate_arg) {

    float* __sum_by_thrs_work_result;

    struct __sum_by_pthread_args { float_array* restrict arr; bool(* predicate)(float, float); float predicate_arg; unsigned long from; unsigned long to; unsigned long chunk_number; };    

    void* __sum_by_pthread(void* args) {
        struct __sum_by_pthread_args* args_st = (struct __sum_by_pthread_args*) args;
        float local_sum = args_st->arr->begin_ptr[args_st->from];
        FOR_RANGE {
            float it = args_st->arr->begin_ptr[i];
                if (args_st->predicate(it, args_st->predicate_arg) == true)
                    local_sum += sin(it);
        }
        __sum_by_thrs_work_result[args_st->chunk_number] = local_sum;
        return NULL;
    };

    float sum = 0;
    #ifdef _OPENMP
    #pragma omp parallel for reduction(+:sum) schedule(static)
    for (unsigned long i = 0; i < arr->size; i++) {
        float it = arr->begin_ptr[i];
        if (predicate(it, predicate_arg) == true)
            sum += sin(it);
    }
    #else
    long num_of_threads = sysconf(_SC_NPROCESSORS_ONLN);
    __sum_by_thrs_work_result = malloc(num_of_threads * sizeof(float));
    PREPARE_PITER {
        struct __sum_by_pthread_args* args = malloc(sizeof(struct __sum_by_pthread_args));
        args->arr = arr;
        args->predicate = predicate;
        args->predicate_arg = predicate_arg;
        args->chunk_number = i;
        CREATE_PITER(__sum_by_pthread);
    }
    JOIN_PITER();
    for (unsigned long j = 0; j < num_of_threads; j++)
        sum += __sum_by_thrs_work_result[j];    
    #endif
    return sum;
}