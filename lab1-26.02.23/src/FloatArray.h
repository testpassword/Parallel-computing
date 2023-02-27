typedef struct float_array {
    float* begin_ptr;
    unsigned long size;
    unsigned long engaged;
} float_array;

float_array FloatArray(unsigned long size);

float_array* push(float_array* arr, float val);

float_array* map(float_array* arr, float(* transform)(float));

float reduce(float_array* arr, float init, float(* transform)(float, float));

float_array* merge(float_array* arr1, float_array* arr2, float(* transform)(float, float));

float_array* print(float_array* arr);

float_array* sort(float_array* arr);
    void swap(float* a, float* b);
    void heapify(float arr[], int N, int i);
