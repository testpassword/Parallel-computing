typedef struct float_array {
    float* begin_ptr;
    unsigned long size;
    unsigned long engaged;
} float_array;

float_array FloatArray(unsigned long size);

float_array* push(float_array* arr, float val);

float_array* map(float_array* arr, float(* transform)(float));

float reduce(float_array* arr, float init, float(* transform)(float, float));

float_array* merge(float_array* from, float_array* to, float(* transform)(float, float));

float_array* fill_rand(float_array* arr, int max, int min);

float_array clone(float_array* arr);

float min(float_array* arr);

float max(float_array* arr);

float_array filter(float_array* arr, bool(* check)(float));

void clean(float_array* arr);

float_array* print(float_array* arr);

float_array* for_each(float_array* arr, void(* action)(float));

float_array* sort(float_array* arr);
    void __swap(float* a, float* b);
    void __heapify(float arr[], int N, int i);
