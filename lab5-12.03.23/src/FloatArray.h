typedef struct float_array {
    float* begin_ptr;
    unsigned long size;
    unsigned long engaged;
} float_array;

struct par_iter {
    long default_chunk_size;
    long last_chunk_size;
    long num_of_thrs;
    pthread_t* thrs;
};

float_array FloatArray(unsigned long size);

struct par_iter get_parallel_iterator(long arr_size);

float_array* push(float_array* restrict arr, float val);

float_array* map(float_array* restrict arr, float(* transform)(float));

float_array* merge(float_array* restrict from, float_array* restrict to, float(* transform)(float, float));

float_array* fill_rand(float_array* restrict arr, int max, int min);

float_array clone(float_array* restrict arr);

float min(float_array* restrict arr);

float_array filter(float_array* restrict arr, bool(* check)(float));

void clean(float_array* restrict arr);

float_array* print(float_array* restrict arr);

float_array* sort(float_array* restrict arr);
    void __swap(float* a, float* b);
    void __heapify(float arr[], int N, int i);
