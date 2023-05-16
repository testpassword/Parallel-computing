// piter is Paraller ITERator
struct piter {
    long default_chunk_size;
    long last_chunk_size;
    long num_of_thrs;
    pthread_t* thrs;
};

struct pair {
    long first;
    long second;
};

struct piter create_piter(long arr_size);

struct pair get_range(struct piter* piter, long i);

#define JOIN_PITER() for (long i = 0; i < piter.num_of_thrs; i++) pthread_join(piter.thrs[i], NULL)

#define CREATE_PITER(f) {\
    struct pair range = get_range(&piter, i);\
    args->from = range.first;\
    args->to = range.second;\
    pthread_create(&piter.thrs[i], NULL, f, (void*) args);\
}

#define PREPARE_PITER \
    struct piter piter = create_piter(arr->size);\
    for (long i = 0; i < piter.num_of_thrs; i++) \

#define FOR_RANGE for (unsigned long i = args_st->from; i < args_st->to; i++)