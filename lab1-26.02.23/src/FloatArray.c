#include <stdio.h>
#include <stdlib.h>


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

float reduce(float_array* arr, float init, float(* transform)(float, float)) {
    float acc = init;
    for (unsigned long i = 0; i < arr->size; i++)
        acc = transform(acc, arr->begin_ptr[i]);
    return acc;
}

float_array* merge(float_array* arr1, float_array* arr2, float(* transform)(float, float)) {
    for (unsigned long i = 0; i < arr2->size; i++) 
        arr2->begin_ptr[i] = transform(arr1->begin_ptr[i], arr2->begin_ptr[i]);
    return arr2;
}

float_array* print(float_array* arr) {
    for (unsigned long i = 0; i < arr->size; i++) 
        printf("%.6f\n", arr->begin_ptr[i]);
    return arr;
}

float_array* sort(float_array* arr) {
    /* I would like the functions to be nested, but this cannot be done in C, so I declared them 
    inside the main function to stylistically show that these functions are not self-contained. */
    void swap(float* a, float* b);
    void heapify(float arr[], int N, int i);
    for (int i = arr->size / 2 - 1; i >= 0; i--) 
        heapify(arr->begin_ptr, arr->size, i);
    for (int i = arr->size - 1; i >= 0; i--) {
        swap(&arr->begin_ptr[0], &arr->begin_ptr[i]);
        heapify(arr->begin_ptr, i, 0);
    }
    return arr;
}
    void swap(float* a, float* b) {
        int temp = *a;
        *a = *b;
        *b = temp;
    }
    void heapify(float arr[], int N, int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < N && arr[left] > arr[largest]) largest = left;
        if (right < N && arr[right] > arr[largest]) largest = right;
        if (largest != i) {
            swap(&arr[i], &arr[largest]);
            heapify(arr, N, largest);
        }
    }