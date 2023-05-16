#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include "PIter.h"


struct piter create_piter(long arr_size) {
    long num_of_cores = sysconf(_SC_NPROCESSORS_ONLN);
    long default_chunk = arr_size / num_of_cores;
    long last_chunk = default_chunk + (arr_size % num_of_cores);
    pthread_t* thrs;
    thrs = malloc(num_of_cores * sizeof(pthread_t));
    return (struct piter){ default_chunk, last_chunk, num_of_cores, thrs };
}

struct pair get_range(struct piter* piter, long i) {
    long from = i * piter->default_chunk_size;
    long to = i * piter->default_chunk_size + (piter->num_of_thrs - 1 != i ? piter->default_chunk_size : piter->last_chunk_size);
    return (struct pair){ from, to };
}