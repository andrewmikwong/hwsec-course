#include "lib_covert.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>

void *allocate_huge_page() {
    void *buf = mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, 
                    MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
    if (buf == MAP_FAILED) {
        perror("mmap huge page failed");
        return NULL;
    }
    return buf;
}

void **setup_eviction_set(void *base, int set_index, int ways) {
    void **pointers[ways];
    
    // Calculate addresses
    for (int w = 0; w < ways; w++) {
        uint64_t offset = (w * L2_STRIDE) + (set_index * CACHE_LINE_SIZE);
        pointers[w] = (void **)((char *)base + offset);
    }

    // Link them circularly: 0->1->...->(ways-1)->0
    for (int w = 0; w < ways; w++) {
        *pointers[w] = pointers[(w + 1) % ways];
    }

    return pointers[0];
}

uint64_t get_time_serializing() {
    uint64_t a, d;
    unsigned int aux;
    asm volatile("rdtscp" : "=a"(a), "=d"(d), "=c"(aux) : : "memory");
    return (d << 32) | a;
}

void shuffle(int *array, size_t n) {
    if (n > 1) {
        for (size_t i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void **traverse_list(void **start_node, int ways) {
    void **p = start_node;
    for (int i = 0; i < ways; i++) {
        p = (void **)*p;
    }
    return p;
}

uint64_t measure_probe_time(void **start_node, int ways) {
    uint64_t t1 = get_time_serializing();
    traverse_list(start_node, ways);
    uint64_t t2 = get_time_serializing();
    return t2 - t1;
}

void hammer_set(void **start_node, int ways, uint64_t duration_cycles) {
    uint64_t start = get_time_serializing();
    void **p = start_node;
    
    // Unrolled loop for maximum memory pressure
    while (get_time_serializing() < start + duration_cycles) {
        p = (void **)*p;
        p = (void **)*p;
        p = (void **)*p;
        p = (void **)*p;
        p = (void **)*p;
    }
}
