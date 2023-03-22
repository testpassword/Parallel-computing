#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdbool.h>
#include "FW_1.3.1_Lin64/fwBase.h"
#include "FW_1.3.1_Lin64/fwSignal.h"


#define NUM_OF_EXPEREMENTS 100
#define A 392
__thread unsigned int seed = 0;


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

float_array* push(float_array* arr, float val) {
    arr->begin_ptr[arr->engaged] = val;
    arr->engaged = arr->engaged + 1;
    return arr;
}

float_array* map(float_array* arr, float(* transform)(float)) {
    for (unsigned long i = 0; i < arr->size; i++)
        arr->begin_ptr[i] = transform(arr->begin_ptr[i]);
    return arr;
}

float_array* fill_rand(float_array* arr, int max, int min) {
    for (unsigned long i = 0; i < arr->size; i++) {
        seed = i;
        arr->begin_ptr[i] = (max <= min || abs(max) == abs(min)) ? 0 : min + rand_r(&seed) % ((max + 1) - min);
    }
    return arr;
}

float_array filter(float_array* arr, bool(* check)(float)) {
    unsigned long new_arr_size = 0;
    for (unsigned long i = 0; i < arr->size; i++) 
        if (check(arr->begin_ptr[i]) == true) 
            new_arr_size++;
    float_array new = FloatArray(new_arr_size);
    for (unsigned long i = 0; i < arr->size; i++) 
        if (check(arr->begin_ptr[i]) == true) 
            push(&new, arr->begin_ptr[i]);
    return new;
}

float min(float_array* arr) {
    float min = arr->begin_ptr[0];
    for (unsigned long i = 0; i < arr->size; i++) {
        float it = arr->begin_ptr[i];
        if (it < min) min = it;
    }
    return min;
}

float_array* print(float_array* arr) {
    printf("[");
    for (unsigned long i = 0; i < arr->size; i++) {
        printf("%.6f", arr->begin_ptr[i]);
        if (i != arr->size - 1) printf(", ");
    }
    printf("]\n");
    return arr;
}

float_array* sort(float_array* arr) {
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


float to_negative0_1(float x) { return x / 1000 * -1; }  // special function discussed personally with the teacher 
float abs_sin(float val1, float val2) { return fabs(sin(val1 + val2)); }
bool not_null(float val) { return val != 0 && val != NAN; }


int main(int argc, char* argv[]) {    
    if (argc < 2) return -1;
    struct timeval T1, T2;
    const int N = atoi(argv[1]);
    const int M = atoi(argv[2]);
    fwSetNumThreads(M);
    printf("test flight\n");
    gettimeofday(&T1, NULL);
    for (int i = 0; i < NUM_OF_EXPEREMENTS; i++) {
        // GENERATE
        float_array M1 = FloatArray(N);
        float_array M2 = FloatArray(N / 2);
        float_array M2_COPY = FloatArray(N / 2);
        fill_rand(&M1, A, 1);
        map(&M1, to_negative0_1); // special transform discussed personally with the teacher 
        fill_rand(&M2, 10 * A, A);
        // MAP
        fwsTanh_32f_A11(M1.begin_ptr, M1.begin_ptr, M1.size);
        fwsAddC_32f(M1.begin_ptr, 1, M1.begin_ptr, M1.size);
        fwsCopy_32f(M2.begin_ptr, M2_COPY.begin_ptr, M2.size);

        // TODO: заменить этот цикл
        for (unsigned long i = 0; i < M2.size; i++) {
            float m2i_cur = M2_COPY.begin_ptr[i];
            float m2i_prev = i == 0 ? 0 : M2_COPY.begin_ptr[i - 1];
            M2.begin_ptr[i] = abs_sin(m2i_cur, m2i_prev);
        }

        fwFree(M2_COPY.begin_ptr);
        // MERGE
        fwsMinEvery_32f_I(M1.begin_ptr, M2.begin_ptr, M2.size);
        // SORT
        sort(&M2);
        // REDUCE
        float_array not_null_arr = filter(&M2, not_null);
        float min_not_null = min(&not_null_arr);
        float sum = 0;
        for (unsigned long i = 0; i < M2.size; i++) {
            float it = M2.begin_ptr[i];
            if ((int)(it / min_not_null) == 0)
                sum += sin(it);
        }
        free(M1.begin_ptr);
        free(M2.begin_ptr);
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
